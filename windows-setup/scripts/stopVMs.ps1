$VMs = "r1 r2 r3 h1 h2 h3 h4 h5".Split(" ")
For ($i = 0; $i -lt $VMs.Length; $i++) {
    Write-Host "Stopping VM :" $VMs[$i]
    VBoxManage controlvm $VMs[$i] poweroff
    Start-Sleep 1
}