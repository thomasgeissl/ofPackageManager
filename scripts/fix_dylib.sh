#!/bin/sh
cwd=$(pwd)
cd "$(dirname "$0")"

cp ../local_addons/ofxGit2/libs/libgit2/lib/osx/libgit2.1.5.dylib ../bin/ofPackageManager.app/Contents/MacOS/
cd ../bin/ofPackageManager.app/Contents/MacOS/
install_name_tool -change @rpath/libgit2.1.5.dylib @executable_path/libgit2.1.5.dylib ./ofPackageManager

cd $pwd

# #!/bin/sh
# cwd=$(pwd)
# cd "$(dirname "$0")"

# cp ../local_addons/ofxGit2/libs/libgit2/lib/osx/libgit2.1.1.dylib ../bin/ofPackageManager.app/Contents/MacOS/
# install_name_tool -change @rpath/libgit2.1.1.dylib @executable_path/libgit2.1.1.dylib ../bin/ofPackageManager.app/Contents/MacOS/ofPackageManager

# cd $pwd