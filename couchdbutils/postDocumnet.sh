SERVER=http://smartt.iriscouch.com:5984
SERVER=http://localhost:5984
SERVER=http://cloud.house4hack.co.za:8888
SERVER=http://corrie206.co.za/couchdbroot

cd /home/schalk/Projects/Nasa/smartt

curl -X DELETE $SERVER/smartt
couchapp push $SERVER/smartt
curl -i -X POST -H 'Content-Type: application/json' -d '{"type":"weather","id":"W123","temperature":0.5, "lat":-25.746559, "lon":28.267908}'  $SERVER/smartt/_design/smartt/_update/sensor/

curl -i -X POST -H 'Content-Type: application/json' -d '{"type":"weather","id":"W123","temperature":0.5, "lat":-26.05, "lon":27.90}'  $SERVER/smartt/_design/smartt/_update/sensor/

curl -i -X POST -H 'Content-Type: application/json' -d '{"type":"weather","id":"W123","temperature":145,"humidity":138,"light":130, "power":134, "cox":130,"rain":129, "lat":-25.85, "lon":27.50}'  $SERVER/smartt/_design/smartt/_update/sensor/




curl -i -X POST -H 'Content-Type: application/json' -d '{"type":"power","id":"P123","power":"on","lat":-25.780107, "lon":28.275375}'  $SERVER/smartt/_design/smartt/_update/sensor/


# not working
curl -X PUT -H 'Content-Type: application/json' -d '{"from": "","to": "sensor","method": "*","query": {}}' http://localhost:5984/smartt/_design/smartt/_rewrite/rule1

curl -X GET http://smartt.iriscouch.com:5984/smartt/_design/smartt/_view/location

curl -X GET http://smartt.iriscouch.com:5984/smartt/_design/smartt/_view/timestamp?descending=true&limit=10



