cmd /c npm run-script build
cd ../../mpcomplete.github.io
rm -rf blingbling/*
cp -R ../blingbling/web/dist/* ./blingbling/
cp -R ../blingbling/web/assets/* ./blingbling/assets/
git add blingbling
@REM git ci -am "Update to latest"
@REM git push -u origin master
cd ../blingbling/web