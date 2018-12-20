#!/bin/sh

#!/bin/sh

curl -O curl https://openframeworks.cc/versions/v0.10.1/of_v0.10.1_msys2_release.zip
unzip -a -qq of_v0.10.1_msys2_release.zip
rm of_v0.10.1_msys2_release.zip
./of_v0.10.1_msys2_release/scripts/msys2/install_dependencies.sh

cd of_v0.10.1_msys2_release/apps
mkdir ofPackageManager && cd ofPackageManager
git clone https://github.com/thomasgeissl/ofPackageManager.git
cd ofPackageManager/scripts
./install_dependencies.sh
cd ..
make