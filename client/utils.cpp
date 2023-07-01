#include "utils.h"


std::string* utils::readFile() {
	std::string* detials = new std::string[4];
	std::string content;
	int index;
	std::ifstream ReadFile("transfer.info.txt");
	if (!ReadFile) {
		std::cout << "cannt open file";
		// throw exeption
	}

	getline(ReadFile, content);
	index = content.find(":");
	detials[0] = content.substr(0, index);
	detials[1] = content.substr(index + 1);
	getline(ReadFile, detials[2]);
	getline(ReadFile, detials[3]);
	ReadFile.close();
	return detials;
}

std::string utils::readMeFile(int f) {
	std::string content;
	std::string file;
	try
	{
		std::ifstream ReadFile("me.info.txt");
		if (!ReadFile) {
			std::cout << "cannt open file";
		}
		int i = 0;
		while (getline(ReadFile, content))
		{
			if (f == 0) {
				if (i == 1) 
					file = content;
			}
			else
			{
				if (i > 1) 
					file += content;
			}
			i += 1;
		}
		ReadFile.close();
		
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
	return file;
	
}


std::string utils::decode_uuid(std::string uuid) {

	std::string output_string;

	for (int i = 0; i < uuid.length(); i += 2) {
		std::string byte_string = uuid.substr(i, 2);
		int byte = std::stoi(byte_string, nullptr, 16);
		output_string += static_cast<char>(byte);
	}
	return output_string;
}

std::string* utils::readFile(std::string file_name) {
	std::string* detials = new std::string[3];
	std::string content;

	std::ifstream ReadFile(file_name);
	if (!ReadFile) {
		std::cout << "cannt open file";
		
	}

	int index = 0;
	while (getline(ReadFile, content))
	{
		detials[index] = content;
		index++;
	}
	ReadFile.close();
	return detials;
}




void utils::writeToFile(std::string file_name, std::string content) {


		std::fstream file;
		file.open(file_name, std::ios::app);
		if (!file)
			std::cout << "Can't open me.info file";
			//throw ExceptionConfigFile("Can't open me.info file");
		file << content;
		file.close();
	
}

bool utils::exist_meInfo() {

	std::ifstream ifile;
	ifile.open("me.info.txt");
	if (ifile) {//if file exist re register
		ifile.close();
		return true;
	}
	else {//file dos'nt exist register
		return false;
	}

	

}

void utils::uuid_to_hex(const unsigned char* buffer, unsigned int length)
{

	std::ios::fmtflags f(std::cout.flags());
	std::cout << std::hex;

	std::ofstream fout("me.info.txt", std::ios::app);
	std::streambuf* coutbuf = std::cout.rdbuf(); //save old buf
	std::cout.rdbuf(fout.rdbuf()); //redirect std::cout to out.txt!

	std::cout << "\n";

	for (size_t i = 0; i < length; i++)
		std::cout << std::setfill('0') << std::setw(2) << (0xFF & buffer[i]) << (((i + 1) % 16 == 0) ? "" : "");
	std::cout << std::endl;
	std::cout.flags(f);
	std::cout.rdbuf(coutbuf); //reset to standard output again
}

void utils::random_client_id(unsigned char* buf, int size)
{
	srand(time(NULL));

	int low = 'A';
	int high = 'z';

	for (size_t i = 0; i < size; i++)
		buf[i] = low + std::rand() % (high - low + 1);
}


void utils::add_null_terminating_byte(std::string str, char* buf)
{
	std::vector<char> writable(str.begin(), str.end());
	writable.push_back('\0'); //Adding null-terminated byte

	std::memcpy(buf, &writable[0], str.length() + 1);

}


std::string utils::readFileClient(std::string file_name) {

	std::ifstream infile(file_name);

	infile.seekg(0, std::ios::end);
	size_t length = infile.tellg();
	infile.seekg(0, std::ios::beg);

		char* buffer = new char[length];
	infile.read(buffer, length);

	std::string final_buffer(buffer, length);
	/*delete buffer;*/
	return final_buffer;

		
}



uint32_t utils::get_crc32(const std::string& my_string)
{
	boost::crc_32_type result;
	result.process_bytes(my_string.data(), my_string.length());
	return result.checksum();
}