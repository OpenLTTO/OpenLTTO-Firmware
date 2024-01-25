This component should implement all functionality of the Combobulator Host firmware, allowing the device to act as an IR Relay and LTTO Game Host Device.

Essentially in the beginning for simplicity we can port the existing Combobulator host code here.

In the future, we should rely on porting the hosting "Game Logic" into the Game component, and this only handles the wifi service needed for communicating/managing the game via the Combobulator app, relying on the underlying Game classes for actual functionality.
