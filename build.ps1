Push-Location .
$build_dir = 'build'

if (-Not (Test-Path -Path $build_dir)) {
  mkdir $build_dir
}

Set-Location $build_dir

$threads = (Get-CimInstance Win32_ComputerSystem).NumberOfLogicalProcessors

cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug -j $threads

if (-Not ($LASTEXITCODE -eq "0")) {
  Write-Output "cmake --build for Debug mode failed!"
  Pop-Location
  exit 1
}

Pop-Location
exit 0

