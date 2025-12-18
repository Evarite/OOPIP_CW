#include "DeliverySystem.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace DeliverySystem
{
	TablePrinter::TablePrinter(const std::vector<std::string>& headers) : headers(headers) {}

	void TablePrinter::AddRow(const std::vector<std::string>& row)
	{
		rows.push_back(row);
	}

	std::ostream& operator<<(std::ostream& os, const TablePrinter& printer)
	{
		if (printer.headers.empty())
			return os;

		std::vector<std::vector<std::vector<std::string>>> splitRows;
		std::vector<size_t> maxLinesPerRow;

		for (const auto& row : printer.rows)
		{
			std::vector<std::vector<std::string>> splitRow;
			size_t maxLinesInRow = 1;

			for (const auto& cell : row) 
			{
				std::vector<std::string> lines;
				std::stringstream ss(cell);
				std::string line;

				while (std::getline(ss, line, '\n'))
					lines.push_back(line);

				if (lines.empty())
					lines.push_back("");

				splitRow.push_back(lines);
				maxLinesInRow = std::max(maxLinesInRow, lines.size());
			}

			splitRows.push_back(splitRow);
			maxLinesPerRow.push_back(maxLinesInRow);
		}

		std::vector<size_t> columnWidths(printer.headers.size(), 0);

		for (size_t i = 0; i < printer.headers.size(); ++i)
		{
			std::stringstream ss(printer.headers[i]);
			std::string line;
			while (std::getline(ss, line, '\n'))
				columnWidths[i] = std::max(columnWidths[i], line.size());
		}

		for (const auto& splitRow : splitRows) 
			for (size_t i = 0; i < std::min(splitRow.size(), printer.headers.size()); ++i) 
				for (const auto& line : splitRow[i]) 
					columnWidths[i] = std::max(columnWidths[i], line.size());

		for (auto& width : columnWidths)
			width += 2;

		auto PrintHorizontalLine = [columnWidths, &os](const char* left, const char* middle, const char* right)
			{
			os << left;
			for (size_t i = 0; i < columnWidths.size(); ++i) 
			{
				os << std::string(columnWidths[i], '-');
				if (i < columnWidths.size() - 1)
					os << middle;
			}
			os << right << '\n';
			};

		PrintHorizontalLine("+", "+", "+");

		std::vector<std::vector<std::string>> splitHeaders;
		size_t maxLinesInHeader = 1;

		for (const auto& header : printer.headers) 
		{
			std::vector<std::string> lines;
			std::stringstream ss(header);
			std::string line;

			while (std::getline(ss, line, '\n')) 
				lines.push_back(line);

			if (lines.empty()) 
				lines.push_back("");

			splitHeaders.push_back(lines);
			maxLinesInHeader = std::max(maxLinesInHeader, lines.size());
		}

		for (size_t lineIndex = 0; lineIndex < maxLinesInHeader; ++lineIndex)
		{
			os << "|";
			for (size_t i = 0; i < printer.headers.size(); ++i) 
			{
				std::string cellLine = (lineIndex < splitHeaders[i].size())
					? splitHeaders[i][lineIndex] : "";
				os << " " << std::left << std::setw(columnWidths[i] - 1) << cellLine << "|";
			}
			os << '\n';
		}

		PrintHorizontalLine("+", "+", "+");

		for (size_t rowIndex = 0; rowIndex < splitRows.size(); ++rowIndex)
		{
			const auto& splitRow = splitRows[rowIndex];
			size_t maxLinesInRow = maxLinesPerRow[rowIndex];

			for (size_t lineIndex = 0; lineIndex < maxLinesInRow; ++lineIndex) 
			{
				os << "|";
				for (size_t i = 0; i < printer.headers.size(); ++i) 
				{
					std::string cellLine;
					if (i < splitRow.size() && lineIndex < splitRow[i].size()) 
						cellLine = splitRow[i][lineIndex];

					os << " " << std::left << std::setw(columnWidths[i] - 1) << cellLine << "|";
				}
				os << '\n';
			}

			if (rowIndex < splitRows.size() - 1) 
				PrintHorizontalLine("+", "+", "+");
		}

		PrintHorizontalLine("+", "+", "+");

		return os;
	}
}