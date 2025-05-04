# Manual
## Operation
At startup, the landing screen will be the configuration menu. After selecting a configuration, you will be taken to the main screen.
### Main Screen
The main screen will show the weight from the load cell, and if the number of load cells allows, left/right load balance and front/back load balance. The total weight from adding all the load cells together is then shown.
The layout shown on the screen corresponds to the physical layout of the load cells in order for the left/right and front/back balance to be accurate. 
The load cells shown on the top of the screen are classified as front for the purpose of the balance, the load cells shown on the left side of the screen are calculated as left.
Pressing the menu button will go to the calibration menu. Pressing shift will go to the configuration menu. Plus will not do anything on this screen. Pressing minus will tare all the configured load cell.
### Calibration Menu
Use the buttons to select which load cell to calibrate. Note, you will only be able to select a load cell that has been enabled through the configuration menu. If you want to cancel the calibration, on the 'Starting Calibration' screen, press the menu button to return to the main screen. To continue, press the minus button to tare. While tareing there should be no weight on the load cell, or what you would like to read as 0 on the scale. 
After tareing, press menu to continue and start the calibration. Load the load cell with a known mass and enter the value of the mass. Shift will move one digit to the right, Plus will increase by a digit, Minus will decrease by a digit. Press Menu to confirm. You will then be given the option to save the calibration, choose yes or no. If No is selected, the previous calibration value will be retained and you will be returned to the main screen. If yes is selected, the new calibration value will be saved to EEPROM and used immediately.
### Configuration Menu
This screen is to choose the number of load cells that are connected. This will also determine which version of the main screen is to be displayed, and which load cells can be calibrated. While this screen is displayed, it is safe to plug in and unplug load cells. The HX711 load cell amplifiers are in a powered down state until a configuration is selected.
** Please Note **
Choosing the configuration will determine which load cells you can use. For example, choosing one connected load cell will require the use of connection number one. Choosing two will require using connections one and two.
### Tareing
This screen has no interaction, it just shows the status of tareing the load cells. This screen is brought up when pressing the Minus button on the main screen, or any time the configuration is changed.
### Buttons
<img width="1446" alt="Screenshot 2025-05-03 at 7 48 53 PM" src="https://github.com/user-attachments/assets/c98ee8d6-925b-47e0-9829-c2810375219b" />
Menu           Shift           Plus           Minus
