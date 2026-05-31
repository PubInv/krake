# KRAKE cleaned web bundle

Drop these files into the GPAD LittleFS web/static filesystem. Keep the backend endpoints the same:

- `/status`
- `/lcd`
- `/serial-monitor`
- `/wifi`
- `/settings-data`
- `/settings/mute`
- `/settings/wifi/reset`
- `/config`
- `/broker-console/data`
- `/broker-console/publish`

Main cleanup:

- Shared navigation in `js/common.js`
- Shared API helpers in `js/common.js`
- Shared browser broker defaults in `js/common.js`
- Browser MQTT connections can select Krake PubInv or a custom broker, stop automatic retries after an authorization rejection, and optionally clear retained alarm messages
- Device MQTT settings persist a selectable Krake PubInv profile or custom broker host, username, and write-only password
- Operator sound defaults persist volume and mute duration across device restarts
- Navigation points directly to the HTML files stored in this bundle
- PMD topics are loaded from `/settings-data`
- Settings remains the source of truth for MQTT topics
- Device monitor stays responsible for online/offline detection
- CSS consolidated into `style.css`, which the firmware serves directly so style fixes are not shadowed by an older compressed companion
- HTTP responses support Chrome private-network preflights and only send gzip companions when the client advertises gzip support
