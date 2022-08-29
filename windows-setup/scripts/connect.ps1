$vmMap = @{
    h1 = 14501
    h2 = 14502
    h3 = 14503
    h4 = 14504
    h5 = 14505
    r1 = 14601
    r2 = 14602
    r3 = 14603
}

Write-Host "1. Accept Host Key Check if prompted"
Write-Host "2. Use the password 'user@123' to connect to the VM..."
Write-Host ""
$VM = $args[0]
if ($vmMap.ContainsKey($VM)) {
    plink -P $vmMap[$VM] -v tc@localhost -pw user@123
}
else {
    Write-Host "Available VMs are h1, h2, h3, h4, h5, r1, r2, r3"
}