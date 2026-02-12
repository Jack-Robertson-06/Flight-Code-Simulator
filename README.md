# Flight-Code-Simulator

"SendData" is Python script that reads from "flt__0015.csv" (old flight data) and sends the pressure at designated time intervals serially to the Arduino

The arduino runs "FlightCode - ReadData.ino", reads the pressure, turns it back into altitude, and identify key points of the flight

** Biggest bug with the program right now is the csv file has random altitudes of like 30,000 (m) so Apogee gets detected almost instantly, but this can either be accounted for or may be fixed with better flight data in the future **
