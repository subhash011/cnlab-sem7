Write-Host "Copying VM configuration..."
(Get-Content './VMImages/base/base.vbox.template').replace('VPATH', [System.Environment]::CurrentDirectory) | Set-Content '.\VMImages\base\base.vbox'
(Get-Content './VirtualBox/VirtualBox.xml.template').replace('VPATH', [System.Environment]::CurrentDirectory)  | Set-Content './VirtualBox/VirtualBox.xml'
Remove-Item -Recurse $env:VBOX_USER_HOME/*
Copy-Item -Recurse .\VirtualBox\* -Destination $env:VBOX_USER_HOME
