# KRAKE cleaned web bundle

Drop these files into the GPAD LittleFS web/static filesystem. Keep the backend endpoints the same:
Drop these files into your web/static filesystem. Keep the backend endpoints the same.
The firmware serves the source files directly so replacement files cannot be shadowed by stale precompressed copies.

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
- Browser MQTT connections use the private `krakepubinv.cloud.shiftr.io` broker
- Navigation points directly to the HTML files stored in this bundle
- PMD topics are loaded from `/settings-data`
- Settings remains the source of truth for MQTT topics
- Device monitor stays responsible for online/offline detection
- CSS consolidated into `style.css`

## Firmware route aliases

The firmware maps extension-free navigation URLs to their bundle files:

- `/` and `/index.html` → `/index.html` with template processing
- `/manual` → `/manual.html`
- `/PMD_GPAD_API` → `/PMD_GPAD_API.html`
- `/monitor` → `/monitor.html` with template processing
- unknown GET paths → `/404.html`
