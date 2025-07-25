# Summary
A printed wiring assembly for adding an external speaker jack to the Krake.

## Requirements Details.
See issue https://github.com/PubInv/krake/issues/231 (Hardware, Add External Speaker Jack (phone jack), 3,5mm, Switch ) for additional history on how these requirements evolved.

- A PWA which has a 3.5mm phone jack with switched signals.
The KRAKE audio output at J601 is connected into the PWA_EXP_SPK assembly on a 1.27mm JST connector.
When no 3.5mm plug is inserted into the jack, audio is available on an input 1.27mm JST connector for connection to the Krake internal speaker
The assembly is mounted to the Krake jack panel by the panel mount nut of the 3.5mm phone jack.

- Design For Manufacture
Design for manufacture by JLCPCB and PCBWAY.


## Impact On Other Assemblies
- **Front Panel Impact:** The Krake jack panel ?front panel? Must be modified by adding a 6mm hole (with clearance allowance) into which the 3.5mm phone jack will mount.
- **Internal Speaker:** Locate the speaker so that the leads that come with the speaker are long enough, OR build an extension cable assembly to connect to the speaker, OR extend the speaker wires.
