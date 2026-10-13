# Accessibility

## Scope and status

This document describes accessibility considerations for Krake's physical alarm device, configuration interface, and documentation.

 
## Visual and audible alarms

Krake uses sound, lights, and displayed messages to communicate alarm events. Providing several forms of indication can support users with different sensory needs, but their effectiveness requires testing.

- **Hearing access:** Verify that alarm presence, severity, and relevant information can be understood from visual indicators when audio cannot be heard.
- **Vision access:** Verify which information can be understood through audio when the LCD or LEDs cannot be seen. Audio playback alone does not establish spoken access to displayed messages or menus.
- **Color perception:** Use text, indicator position, or other distinguishable cues to communicate severity. Do not rely on color alone.
- **Sensory sensitivity:** Evaluate volume, sound characteristics, and flashing patterns. Record any available adjustments and their limitations.

Any change to audio, visual indications, or mute behavior should be tested to confirm that alarms remain detectable and understandable in the intended environment.

## Display and physical controls

The character LCD has limited space for messages. Readability depends on text clarity, contrast, viewing angle, lighting, placement, and distance.

Recommended checks include:

- Clear wording and understandable abbreviations for alarms and device status.
- Visibility of severity, mute state, and the currently selected action.
- Readability at expected viewing distances and under expected lighting.
- Button and rotary-encoder use by people with limited dexterity or grip strength.
- Clear labels and feedback after a control is operated.
- Ease of distinguishing mute from other alarm actions.
- Reachability of controls after installation or mounting.

Accessible operation of all menus and controls has not been established by this draft.

## Web configuration interface

This section concerns using a computer's web browser and keyboard to access Krake's configuration page. It does not refer to connecting a keyboard to the physical Krake device.

### Open the configuration page

1. Put Krake into Wi-Fi setup mode using the instructions for your firmware version.
2. Connect your computer to Krake's setup Wi-Fi network (typically named `KRAKE_X`).
3. Open a web browser, type `http://192.168.4.1` into the address bar, and press Enter.
4. If you are accessing Krake through your local Wi-Fi network instead, use its assigned IP address, such as `http://<device-IP-address>`, if that firmware version provides configuration access in this mode. Your computer must be on a network that can reach Krake.

Network names, addresses, and configuration availability should be checked against the instructions for the installed firmware version.

### Test keyboard and assistive-technology access

Once the page opens, test whether it can be operated without a mouse. Keyboard and screen-reader support require verification against the interface provided by the relevant firmware version.

Recommended checks include:

- Use Tab to move forward between fields, links, and buttons, and Shift+Tab to move backward.
- Confirm that the focused control has a visible indicator and that focus follows a logical order.
- Type into configuration fields using the keyboard.
- Use Enter to activate links and buttons, and Space to operate buttons or checkboxes where appropriate.
- Confirm that every control is reachable and that focus does not become trapped in any part of the page.
- Descriptive labels for inputs and buttons.
- Logical headings, reading order, and focus order.
- Screen-reader access to connection status, validation errors, and confirmation messages.
- Sufficient contrast and usability when text is enlarged or the browser is zoomed.
- Information and errors that do not depend on color alone.

These steps describe proposed tests; they do not confirm that keyboard or screen-reader support currently works. No WCAG conformance level is claimed in this draft.

## Documentation

Accessibility goals for documentation include clear headings, descriptive links, text alternatives for meaningful images, and written explanations of controls and alarm indications. Instructions should distinguish implemented behavior from planned features and identify the hardware and firmware versions they describe.

## Known gaps and proposed improvements

This draft does not include recorded accessibility test results. Priorities for evaluation are:

1. Understanding alarms without hearing the speaker or seeing the display.
2. LCD readability and physical-control usability.
3. Keyboard and screen-reader access to web configuration.
4. Sound and flashing characteristics for users with sensory sensitivities.
5. Documentation clarity and feedback from users with disabilities.

These are proposed evaluation priorities, not committed release features. As testing is completed, replace general statements with specific results, limitations, and links to relevant issues.

## Report an accessibility barrier

Please [open an issue in the Krake repository](https://github.com/PubInv/krake/issues/new) and include “Accessibility” in the title.

Useful details include:

- Hardware revision and firmware version, if known.
- The affected control, alarm indication, page, or instruction.
- Steps to reproduce the difficulty, expected behavior, and actual behavior.
- Browser and assistive technology, if relevant.
- A suggested improvement, if you have one.

You do not need to disclose a diagnosis or personal medical information. Public issues are visible to everyone; describe the barrier without including sensitive personal information.

## Contributing

Accessibility improvements and user feedback are welcome. Include the tested hardware or firmware version and explain how the change improves access. Where possible, include results from testing with relevant assistive technologies or users with disabilities.
