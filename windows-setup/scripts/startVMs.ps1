$VMs = "r1 r2 r3 h1 h2 h3 h4 h5".Split(" ")
Write-Host "Starting the VMs..."
For ($i = 0; $i -lt $VMs.Length; $i++) {
    VBoxManage startvm $VMs[$i] -type headless
    Start-Sleep 2
}