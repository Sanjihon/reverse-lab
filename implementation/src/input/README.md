# Input Adapter Boundary

`InputManager` consumes normalized device events. A platform adapter is responsible for translating SDL3 keyboard, gamepad, joystick, hotplug, button, and axis events into `InputEvent` values. This module does not include SDL headers so it remains headless-testable.
