#!/usr/bin/env python3

# Ensure environmental variables i.e. paths are set to the named the modules
from xf_fintech_python import DeviceManager, CFB76, OptionType
import sys

# Basic checking that the number of arguments are correct
if len(sys.argv) != 2:
    sys.exit("Incorrect number of arguments supplied - 1 expected - the name of the FPGA load - e.g. b76.xclbin")

# State test financial model
print("\nThe CFBlack 76 financial model\n==================================================\n")

# Declaring Variables
deviceList = DeviceManager.getDeviceList("u200")
lastruntime = 0
# Example financial data to test the module - same as used in the C++ test script
numAssets = 1000
# Inputs
forwardPriceList = [100.0] * numAssets
strikePriceList = [100.0] * numAssets
volatilityList = [0.1] * numAssets
riskFreeRateList= [0.025] * numAssets
timeToMaturityList = [1.0] * numAssets
# Outputs - declaring them as empty lists
optionPriceList = []
deltaList = []
gammaList = []
vegaList = []
thetaList = []
rhoList = []


# Identify which cards are installed and choose the first available u200 card, as defined in deviceList above
print("Found these {0} device(s):".format(len(deviceList)))
for x in deviceList:
    print(x.getName())
print("Choosing the first suitable card\n")
chosenDevice = deviceList[0]

# Selecting and loading into FPGA on chosen card the financial model to be used
CFB76 = CFB76(numAssets,sys.argv[1])   # warning the lower levels to accomodate at least this figure
CFB76.claimDevice(chosenDevice)
#Feed in the data and request the result using tolerance method
print("\nRunning...")
result = CFB76.run(forwardPriceList, strikePriceList, volatilityList, riskFreeRateList, timeToMaturityList, 
                            optionPriceList, deltaList, gammaList, vegaList, thetaList, rhoList, OptionType.Put, numAssets)
print("Done")
runtime = CFB76.lastruntime()

#Format output to match the example in C++, simply to aid comparison of results
print("+-------+-----------+----------------+--------------+---------------+---------------+---------------+")
print("| Index | Price     |     Delta      |     Gamma    |     Vega      |     Theta     |     Rho       |")
print("+-------+-----------+----------------+--------------+---------------+---------------+---------------+")
for loop in range(0, numAssets) :
    print(loop,"\t%9.5f"%optionPriceList[loop],"\t%9.5f"%deltaList[loop],"\t%9.5f"%gammaList[loop],"\t%9.5f"%vegaList[loop],"\t%9.5f"%thetaList[loop],"\t%9.5f"%rhoList[loop])


print("\nThis run took", str(runtime), "microseconds")

#Relinquish ownership of the card
CFB76.releaseDevice()
