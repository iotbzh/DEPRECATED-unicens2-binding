------------------------------------------------------------------------
   AudioLogic High Level APIs
------------------------------------------------------------------------

Testing: (from project directory bindings)
 * start binder:  ~/opt/bin/afb-daemon --ldpaths=./build --token=mysecret --roothttp=htdocs
 * connect browser on http://localhost:1234?devid=hw:0

 # Open Sound Card from its name
 http://localhost:1234/api/audio/open?token=mysecret&sndname=H650e

 # Subscribe event for a given board
 http://localhost:1234/api/audio/subscribe?token=mysecret&devid=hw:0

 # Increase Volume
 http://localhost:1234/api/audio/setvol?token=mysecret&devid=hw:0&pcm=master&vol=50%

 # Get Volume
 http://localhost:1234/api/audio/getvol?token=mysecret&devid=hw:0&pcm=master

 # Close Session
 http://localhost:1234/api/audio/close?token=mysecret


Testing with afb-client-demo

```
~/opt/bin/afb-client-demo localhost:1234/api?token=mysecret
alsacore subctl {"devid":"hw:0"}
```
 
Start AlsaMixer and change volume
```
alsamixer -D hw:0
```