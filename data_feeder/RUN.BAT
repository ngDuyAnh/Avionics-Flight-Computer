@ECHO OFF
call pip install -r requirements.txt
call python datafeeder.py --serial-port=COM5 --baud-rate=115200 --csv-file=data.csv
