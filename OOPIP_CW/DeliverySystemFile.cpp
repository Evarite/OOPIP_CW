#include "DeliverySystem.h"
#include <fstream>
#include <filesystem>

namespace DeliverySystem
{
	/*std::string name;
	std::fstream* file;*/

	/*void File::Open()
	{
		file->open(name, std::ios::binary | std::ios::in | std::ios::out);
		if (!file->is_open())
		{
			throw std::runtime_error("Не атрымалася адкрыць файл: " + name);
		}
	}
	unsigned int File::GetSize()
	{
		file->seekg(0, std::ios::end);
		return static_cast<unsigned int>(file->tellg());
	}
	void File::Truncate(std::streamsize newSize)
	{
		file->close();
		try
		{
			std::filesystem::resize_file(name, newSize);
		}
		catch (const std::filesystem::filesystem_error & ex)
		{
			throw std::runtime_error("Не атрымалася змяніць памер файла: " + std::string(ex.what()));
		}

		Open();
	}

	File::File(const std::string& name) : name(name)
	{
		file = new std::fstream;
		Open();
	}
	File::~File()
	{
		if (file->is_open())
		{
			file->close();
		}

		delete file;
	}*/

//	template<AllowedType T>
//	void File::WriteVector<T>(const std::vector<T>& vec)
//	{
//		if constexpr (std::is_same_v<T, City>)
//		{
//
//		}
//		if constexpr (std::is_same_v<T, Country>)
//		{
//
//		}
//		if constexpr (std::is_same_v<T, Account>)
//		{
//
//		}
//		if constexpr (std::is_same_v<T, Lorry>)
//		{
//
//		}
//		if constexpr (std::is_same_v<T, Driver>)
//		{
//
//		}
//		if constexpr (std::is_same_v<T, Delivery>)
//		{
//
//		}
//		if constexpr (std::is_same_v<T, Cargo>)
//		{
//
//		}
//		if constexpr (std::is_same_v<T, Trailer>)
//		{
//
//		}
//	}
//
//	template<AllowedType T>
//	void File::ReadVector(std::vector<T>& buf)
//	{
//
//	}
//
//	template<AllowedType T>
//	void File::DeleteRecord(unsigned int index)
//	{
//
//	}
//
//	template<AllowedType T>
//	void File::AddRecord(const T& rec)
//	{
//
//	}
//
//	template<AllowedType T>
//	void File::EditRecord(unsigned int index, const T& newRec)
//	{
//
//	}
}