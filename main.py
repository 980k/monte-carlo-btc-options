import requests
import json
from datetime import datetime as dt
import numpy as np
import scipy.stats as stats
import matplotlib.pyplot as plt
import montecarlo as mc

def list_call_options(spotPrice):
    for contract in response_deriv["data"]:
        contract_string = contract["label"].split('-')

        if(float(contract_string[3]) >= spotPrice):
          print(contract["label"] + "(" + str(contract["id"]) +")")

def visualize_convergence(call_value, stdError):
    x1 = np.linspace(call_value-3*stdError, call_value-1*stdError, 100)
    x2 = np.linspace(call_value-1*stdError, call_value+1*stdError, 100)
    x3 = np.linspace(call_value+1*stdError, call_value+3*stdError, 100)

    s1 = stats.norm.pdf(x1, call_value, stdError)
    s2 = stats.norm.pdf(x2, call_value, stdError)
    s3 = stats.norm.pdf(x3, call_value, stdError)

    plt.fill_between(x1, s1, color='tab:blue',label='> StDev')
    plt.fill_between(x2, s2, color='cornflowerblue',label='1 StDev')
    plt.fill_between(x3, s3, color='tab:blue')

    plt.plot([call_value,call_value],[0, max(s2)*1.1], 'k',
        label='Theoretical Value')
    plt.plot([marketValue,marketValue],[0, max(s2)*1.1], 'r',
        label='Market Value')

    plt.ylabel("Probability")
    plt.xlabel("Option Price")
    plt.legend()
    plt.show()

if __name__=="__main__":

    url_derivatives = "https://api.ledgerx.com/trading/contracts?active=true&contract_type=call&derivative_type=options_contract&asset=CBTC"

    headers = {"Accept": "application/json"}

    response_deriv = requests.get(url_derivatives, headers=headers).json()

    url_spot = "https://ftx.us/api/markets/BTC/USD"

    response_spot = requests.get(url_spot).json()

    spotPrice = response_spot["result"]["last"]
    lowestContractPrice = round(spotPrice, -3)

    strikePrice, rate, time, div, marketValue = 0, 0, 0, 0, 0
    N, M = 10, 1000

    listOptions = input("List all options? [y/n]: ")

    if(listOptions == "y"):
        list_call_options(spotPrice)

    contractId  = input("Enter contract ID: ")
    url_contract = "https://api.ledgerx.com/trading/trades/global?mine=false&should_exclude_busted=true&contract_id=" + contractId + "&derivative_type=options_contract&asset=CBTC&limit=1"
    response_contract = requests.get(url_contract, headers=headers).json()
    marketValue = (response_contract["data"][0]["filled_price"]) * 0.01

    for contract in response_deriv["data"]:
        if(contract["id"] == int(contractId)):
            strikePrice = contract["strike_price"] * (10**-2)

            expire_time = contract["date_expires"].split()
            expire_date = expire_time[0].split("-")
            expire_day = expire_time[1].split(":")
            
            expireYear, expireMonth, expireDay = int(expire_date[0]), int(expire_date[1]), int(expire_date[2])
            expireHour, expireMinutes = 13, int(expire_day[1])
            seconds_in_year = 365.25*24*60*60

            time = ((dt(minute=expireMinutes, hour=expireHour, day=expireDay, month=expireMonth, year=expireYear) - dt.now()).total_seconds()) / seconds_in_year
    
    m = mc.PyMonteCarlo()
    call_value = m.monte_carlo(strikePrice, time, spotPrice, rate, div, marketValue, N, M)
    stdError = m.stdErr
    stdDev = m.stdDev

    ma = mc.PyMonteCarlo()
    call_value2 = ma.monte_carlo_antihetic(strikePrice, time, spotPrice, rate, div, marketValue, N, M)
    stdError2 = ma.stdErr
    stdDev2 = ma.stdDev

    print("Call value is ${0} with SE +/- {1}".format(np.round(call_value,2),np.round(stdError,2)))
    print("Call value is ${0} with SE +/- {1}".format(np.round(call_value2,2),np.round(stdError2,2)))

    visualize_convergence(call_value, stdError)
    visualize_convergence(call_value2, stdError2)