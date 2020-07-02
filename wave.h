enum{
	WAVE_FORMAT_PCM=0x0001,
	WAVE_FORMAT_IEEE_FLOAT=0x0003,
	WAVE_FORMAT_ALAW=0x0006,
	WAVE_FORMAT_MULAW=0x0007,
	WAVE_FORMAT_EXTENSIBLE=0xFFFE
};
typedef unsigned int FOURCC; 
template<char ch0,char ch1,char ch2,char ch3>
struct MakeFOURCC{
	enum{
		value=(ch0<<0)+(ch1<<8)+(ch2<<16)+(ch3<<24)
	};
};
struct Base_chunk{
	FOURCC fcc;
	uint32_t cb_size;
	Base_chunk(FOURCC fourcc){
		fcc=fourcc;
		cb_size=0;
	}
	Base_chunk(){
		fcc=0;
		cb_size=0;
	}
};
struct Wave_format{
	uint16_t format_tag;
	uint16_t channels; 
	uint32_t sample_per_sec;
	uint32_t bytes_per_sec;
	uint16_t block_align;
	uint16_t bits_per_sample;
	uint16_t ex_size;

	Wave_format(){
		format_tag=1; 
		ex_size=0;
		channels=0;
		sample_per_sec=0;
		bytes_per_sec=0;
		block_align=0;
		bits_per_sample=0;
	}

	Wave_format(uint16_t nb_channel,uint32_t sample_rate,uint16_t sample_bits)
		:channels(nb_channel),sample_per_sec(sample_rate),bits_per_sample(sample_bits){
		format_tag=0x01;
		bytes_per_sec=channels*sample_per_sec*bits_per_sample/8;
		block_align=channels*bits_per_sample/8;
		ex_size=0;
	}
};
struct Wave_header{
	std::shared_ptr<Base_chunk> riff;
	FOURCC wave_fcc;
	std::shared_ptr<Base_chunk> fmt;
	std::shared_ptr<Wave_format> fmt_data;
	std::shared_ptr<Base_chunk> data;
	Wave_header(uint16_t nb_channel,uint32_t sample_rate,uint16_t sample_bits){
		riff=std::make_shared<Base_chunk>(MakeFOURCC<'R','I','F','F'>::value);
		fmt=std::make_shared<Base_chunk>(MakeFOURCC<'f','m','t',' '>::value);
		fmt->cb_size=18;
		fmt_data=std::make_shared<Wave_format>(nb_channel,sample_rate,sample_bits);
		data=std::make_shared<Base_chunk>(MakeFOURCC<'d','a','t','a'>::value);
		wave_fcc=MakeFOURCC<'W','A','V','E'>::value;
	}
	Wave_header(){
		riff=std::make_shared<Base_chunk>();
		fmt=std::make_shared<Base_chunk>();

		fmt_data=std::make_shared<Wave_format>();
		data=std::make_shared<Base_chunk>();

		wave_fcc=0;
	}
};
