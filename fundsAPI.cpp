// fundsAPI.cpp : Defines the entry point for the application.
//

#include "fundsAPI.h"
#include <iostream>
#include "cpr/cpr.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <conio.h>
#include <windows.h>

using json = nlohmann::json;

int main()
{
	
	cpr::Url financeYahooUrl = cpr::Url ("https://query1.finance.yahoo.com/v8/finance/chart/BZ=F");
	cpr::Parameters yahooParams = { {"range","1y"}, {"interval","1d"}};
	cpr::Header yahooHeader{{"User-Agent" , "Mozilla/5.0"}};

	cpr::Response response = cpr::Get(
		financeYahooUrl,
		yahooParams,
		yahooHeader);


	if (response.status_code == 200)
	{
		json responseJson;
		
		responseJson =  json::parse(response.text);
		auto jsonPath = responseJson["chart"]["result"][0]["indicators"]["quote"][0]["close"];

		std::vector<double> Prices;
		std::vector<double> MovingAverages;


		for (auto& i : jsonPath)
		{
			if (i.is_null() == false)
			{
				Prices.push_back(i.get<double>());
			}
		}
									

		double sum = 0.0f;

		for (int i = 0; i < 50; i++)
		{
			sum += Prices.at(i);
		}

		double firstMovingAvg = sum / 50.0f;
		MovingAverages.push_back(firstMovingAvg);

		for (int i = 50; i < Prices.size(); i++)
		{
			sum -= Prices.at(i - 50);
			sum += Prices.at(i);

			double newAvg = sum / 50.0f;
			MovingAverages.push_back(newAvg);
		}



		int i = MovingAverages.size() - 1;

		float yesterdaysPrice = Prices[i - 1 + 49];
		float yesterdaysAvg = MovingAverages[i - 1];
		float todaysPrice = Prices[i + 49];
		float todaysAvg = MovingAverages[i];

		if (yesterdaysPrice < yesterdaysAvg && todaysPrice > todaysAvg)
		{
			std::cout << "bullish: buy" << std::endl;
		}
		else if (yesterdaysPrice> yesterdaysAvg && todaysPrice < todaysAvg)
		{
			std::cout << "bearish: sell" << std::endl;
		}
		else
		{
			std::cout << "no changes today" << std::endl;
		}

		std::ofstream outfile("brent_crude_data.csv");
		outfile << "Day,Price,50_Day_MA\n";

		for (int i = 0; i < MovingAverages.size(); i++)
		{
			outfile << i << "," << Prices[i + 49] << "," << MovingAverages[i] << "\n";
		}

		outfile.close();

		// 14-day RSI

		float gains = 0;
		float losses = 0;

		for (int i = Prices.size() - 14; i < Prices.size(); i++)
		{

			double difference = Prices[i] - Prices[i-1];

			if (difference < 0.0f)
			{
				losses += std::abs(difference);
			}
			else if (difference > 0.0f)
			{
				gains += difference;
			}
			else
			{
				losses += 0.0f;
				gains += 0.0f;
			}
		}

		float simpleAvg_gains = gains / 14.0f;
		float simpleAvg_losses = losses / 14.0f;

		float RS = 0.0f;
		float RSI = 0.0f;
		bool panic = 0;
		bool run = 1;

		if (simpleAvg_losses == 0)
		{
			RSI = 100.0f;
		}
		else
		{
			RS = simpleAvg_gains / simpleAvg_losses;

			RSI = 100 - (100 / (1 + RS));
		}
		if (RSI >= 70)
		{
			panic = 1;
			run = 1;

		}
		else
		{
			run = 1;
		}

		// stop-loss engine


		float highest_price_since_buy = 0.0f;
		float risk_percentage = 5.0f;

		std::ifstream high_water("high_water_memory.csv");
		if (high_water.is_open() == 1)
		{
			high_water >> highest_price_since_buy;
			high_water.close();
		}

		if (run == 1)
		{
			if (panic == 1)
			{
				risk_percentage = 1.5f;
			}


			if (Prices.at(Prices.size()-1) > highest_price_since_buy)
			{
				highest_price_since_buy = Prices[Prices.size()-1];
			}

			float stop_loss_calc = highest_price_since_buy - (highest_price_since_buy*(risk_percentage/100.0f));

			if (Prices[Prices.size()-1] <= stop_loss_calc)
			{
				std::cout << "SELL" << std::endl;
			}
			else
			{
				std::cout << "we gucci" << std::endl;
			}

			std::ofstream high_water2("high_water_memory.csv");

			high_water2 << highest_price_since_buy;
			high_water2.close();







		}






	}
	else
	{
		std::cout << "error: " << response.status_code << std::endl;
	}


	std::cout << "press enter to exit";
	
	while (true)
	{

		char ch = _getch();

		if (ch == 13)
		{
			break;
		}

	}





	return 0;
}
