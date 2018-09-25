Param( [String]$Att,
       [String]$Subj,
       [String]$Body
     )

Function Send-EMail {
    Param(
            [Parameter(`
            Mandatory=$true)]
        [String]$To,
            [Parameter(`
            Mandatory=$true)]
        [String]$From,
            [Parameter(`
            Mandatory=$true)]
        [String]$Password,
            [Parameter(`
            Mandatory=$true)]
        [String]$Subject,
            [Parameter(`            Mandatory=$true)]
        [String]$Body,
            [Parameter(`
            Mandatory=$true)]
        [String]$attachment,
        )
}

try {

    $Msg = New-Object System.Net.Mail.MailMessage($From, $To, $Subject, $Body)
    $Srv = "smtp.gmail.com"

    #-ne means not equal
    if($attachment -ne $null) {
        try {
            $Attachments = $attachment -split ("\:\:");

            ForEach($val in $Attachments) {
                $attch = New-Object System.Net.Mail.Attachment($val)
                $Msg.Attachments.Add($attch)
            }
        }
        catch {
            #exit status
            exit 2;
        }

        #587 is gmail specific
        #Searchable on Internet
        $Client = New-Object Net.Mail.SmtpClient($Srv, 587)
        $Client.EnableSsl = $true
        $Client.Credentials = New-Object System.Net.NetworkCredential($From.Split("@")[0], $Password);
        $Client.Send($Msg)
        Remove-Variable -Name Client
        Remove-Variable -Name Password
        #Success exit status
        exit 7;
    }
}
    catch {
        #Different exit status
        exit 3;
}

try {

    Send-EMail
        -attachment $Att
        -To "our.destination@email.address"
        -Body $Body
        -Subject $Subj
        -password "tralalala"
        -From "Address of the sender"
}
catch {
    exit 4;
}
