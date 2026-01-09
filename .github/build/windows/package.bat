mkdir package
pushd package
del /q *
copy ..\bin\Win32\Release\qvmops.exe .\
popd
