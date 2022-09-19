Push-Location .
$build_dir = 'build'

if (-Not (Test-Path -Path $build_dir)) {
  mkdir $build_dir
}

Set-Location $build_dir

ctest --verbose .

if (-Not ($LASTEXITCODE -eq "0")) {
  Write-Output "ctest failed!"
  Pop-Location
  exit 1
}

Pop-Location
exit 0

