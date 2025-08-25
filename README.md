# boenanna-pi
Raspberry Pi controlling a BOE Bot chassis

[Chassis Driver Demonstration](https://youtu.be/iw_Xe_JajT4?si=0qx1W1s7F3d5nseO)

[3D-printed Base](https://www.thingiverse.com/thing:1562194)

# Preparing the Pi:
- Install rpi x64 OS
- Enable ssh
- Enable I2C
- Connect 16-channel servo to board
- Connect servos to appropriate slots (check that they match scripts 0,1,2...)
- Setup your environment for Python/C++ builds and development

# Python Daemon Installation:
> Project uses a virtual environment...

``` shell
python3 -m venv venv #Inside this dir.
source ./venv/bin/activate

pip install -r requirements.txt
```

# [Recommended] C++ Daemon Installation:
Requires [PiPCA9685](https://github.com/barulicm/PiPCA9685)
> Unzip and install as directed so that the lib is in /usr/local/lib &ensp;
> You'll also need cmake, gcc, etc, etc, normal build tools...

```shell
# Uncompress here:
tar -xaf PiPCA9685.tar.xz

# Visit archive
cd PiPCA9685

# Install build deps for PiPCA9685 repo
xargs -a apt_dependencies.txt sudo apt-get install -y

# Installs the PiPCA9685 lib to the system
sudo cmake --workflow --preset install 
```

## Assuming you now have everything installed:
> Everything from here is pretty straightforward. &ensp;
> Take a look at tools/ and examples/ if you want to develop further...

```shell
make
make install

# refresh systemd
systemctl --user daemon-reload

# enable service
systemctl --user enable --now boedriver.service
```
