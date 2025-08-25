all: chassisDaemon

python_config_venv:
	python3 -m venv venv
	venv/bin/python3 -m pip install -r requirements.txt

test_chassis_lib:
	mkdir -p build
	g++ tests/test_chassis_lib.cpp -o build/test_chassis_lib -I cpp/PiBoe/ -L/usr/local/lib -lPiPCA9685 -li2c -Wl,-rpath,/usr/local/lib

chassisDaemon:
	g++ cpp/PiBoe/chassisDaemon.cpp -o build/chassisDaemon -I cpp/piBoe/ -L/usr/local/lib -lPiPCA9685 -li2c -Wl,-rpath,/usr/local/lib

install:
	touch $(HOME)/BOEDRIVER_errlog
	touch $(HOME)/BOEDRIVER_log
	touch $(HOME)/BOE_DRIVER_FILE

	sudo cp build/chassisDaemon /usr/local/bin/
	cp boedriver.service $(HOME)/.config/systemd/user
clean:
	rm -rf build
	rm -rf venv
