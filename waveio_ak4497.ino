
// based http://www.dddac.com/otherprojects_lcd.html

#define COUNT_WAVEIO_PIN_MODE 8
#define COUNT_AK4497_PIN_MODE 6

#define PIN_HOST_WAVEIO 6
#define PIN_AUDIO_WAVEIO 7
#define PIN_MUTE_AK4497 5

typedef struct {
  int num_pin;
  int type_pin; // 0 - diskret input, 1 - analog input, 2 - diskret output
  int mode;
  char name_pin[21];
} PinMode_st;

enum MODE_PCM_DSD {
  MODE_PCM_44_1 = 0,
  MODE_PCM_48,
  MODE_PCM_88_2,
  MODE_PCM_96,
  MODE_PCM_176_4,
  MODE_PCM_192,
  MODE_PCM_352_8,
  MODE_PCM_384,
  MODE_DOP_64,
  MODE_DOP_128,
  MODE_DSD_64,
  MODE_DSD_128,
  MODE_DSD_256,
  NO_MODE,
  MAX_MODE_PCM_DSD,
};

typedef struct {
  char name_mode[21];
  int value_mode_waveio;
  int value_mode_ak4497;
} Mode_PCM_DSD_st;


PinMode_st waveio_pin[COUNT_WAVEIO_PIN_MODE]={
  {8,0,0,"L1_44",},
  {9,0,0,"L2_48",},
  {10,0,0,"L3_88",},
  {11,0,0,"L4_96",},
  {12,0,0,"L5_176",},
  {13,0,0,"L6_192",},
  {A5,1,0,"L7_host",},
  {A4,1,0,"L8_audio",},
};

PinMode_st ak4497_pin[COUNT_AK4497_PIN_MODE]={
  {2,2,0,"FS0",},
  {3,2,0,"FS1",},
  {4,2,0,"FS2",},
  {5,2,0,"FS3",},
  {6,2,0,"DSD",},
  {7,2,0,"MUTE",},
};

Mode_PCM_DSD_st table_convert_modes[MAX_MODE_PCM_DSD]={
                                    //                         DSD FS3 FS2 FS1 FS0
  { "PCM 44.1", B000001, B00001, }, // WAVEIO: PCM 44.1 = L1     0   0   0   0   1
  { "PCM 48",   B000010, B00010, }, // WAVEIO: PCM 48 = L2       0   0   0   1   0
  { "PCM 88.2", B000100, B00011, }, // WAVEIO: PCM 88.2 = L3     0   0   0   1   1
  { "PCM 96",   B001000, B00100, }, // WAVEIO: PCM 96 = L4       0   0   1   0   0
  { "PCM 176.4",B010000, B00101, }, // WAVEIO: PCM 176.4 = L5    0   0   1   0   1
  { "PCM 192",  B100000, B00110, }, // WAVEIO: PCM 192 = L6      0   0   1   1   0
  { "PCM 352.8",B010100, B00111, }, // WAVEIO: PCM 352 = L5 + L3 0   0   1   1   1
  { "PCM 384",  B101000, B01000, }, // WAVEIO: PCM 384 = L6 + L4 0   1   0   0   0
  { "DoP 64",   B000101, B11001, }, // WAVEIO: DoP 64  = L1 + L3 0   0   0   0   0
  { "DoP 128",  B001001, B11010, }, // WAVEIO: DoP 128 = L1 + L4 0   0   0   0   0
  { "DSD 64",   B000110, B11001, }, // WAVEIO: DSD 64  = L2 + L3 1   1   0   0   1
  { "DSD 128",  B001010, B11010, }, // WAVEIO: DSD 128 = L2 + L4 1   1   0   1   0
  { "DSD 256",  B010010, B11011, }, // WAVEIO: DSD 256 = L2 + L5 1   1   0   1   1
  { "NO MODE",  B000000, B00000, },
};
    
const int An_level = 200; // used to check at analog input. 200= ~1V 

int flag_new_mode_wave_io=0, audio_streaming_waveio=0, ptr_mode_waveio=-1;

//#define YES_LOG_SERIAL
#ifdef YES_LOG_SERIAL

void log_dec ( const char * name_value, int value )
{
  Serial.print(name_value);
  Serial.print(": ");
  Serial.print(value,DEC);
  Serial.print(", ");
}  

void log_bin ( const char * name_value, int value )
{
  Serial.print(name_value);
  Serial.print(": ");
  Serial.print(value,BIN);
  Serial.print(", ");
}  

void log_str ( const char * name_value, const char * value )
{
  Serial.print(name_value);
  Serial.print(": ");
  Serial.print(value);
  Serial.print(", ");
}  

void log_ln ( const char * str )
{
  Serial.println(str);
}

#else

#define log_dec(x,y)
#define log_bin(x,y)
#define log_str(x,y)
#define log_ln(x)

#endif

int init_pin_mode ( PinMode_st * pm )
{
  if ( pm==0 )
    return -1;

  switch ( pm->type_pin ) {
  case 0:
    pinMode(pm->num_pin, INPUT);
    break;
  case 1:
    pinMode(pm->num_pin, INPUT);
    break;
  case 2:
    pinMode(pm->num_pin, OUTPUT);
    digitalWrite(pm->num_pin, LOW);
    break;
  default:
    return -1;
  }
  
  return 0;
}

int get_pin_mode ( int & flag_new_mode, PinMode_st * pm )
{
  int old_mode=pm->mode;

  if ( pm==0 )
    return -1;

  switch ( pm->type_pin ) {
  case 0:
    pm->mode=digitalRead(pm->num_pin);
    break;
  case 1:
    pm->mode=analogRead(pm->num_pin)>An_level?1:0;
    break;
  case 2:
    pm->mode=digitalRead(pm->num_pin);
    break;
  default:
    return -1;
  }
  
  if ( pm->mode!=old_mode ) {
   flag_new_mode++;
  }

  return 0;
}

int set_pin_mode ( PinMode_st * pm )
{
  int old_mode=pm->mode;

  if ( pm==0 )
    return -1;

  switch ( pm->type_pin ) {
  case 0:
  case 1:
    return -1;
  case 2:
    digitalWrite(pm->num_pin,pm->mode?HIGH:LOW);
    break;
  default:
    return -1;
  }
  
  return 0;
}

int print_pin_mode ( PinMode_st * pm )
{
  if ( pm==0 )
    return -1;
  log_dec(pm->name_pin,pm->mode);
}

int  set_bit ( int number, int n_bit )
{
  if ( n_bit<1 || n_bit>32 ) return number;
  return number | (1 << (n_bit-1));
}

int  get_bit ( int number, int n_bit )
{
  if ( n_bit<1 || n_bit>32 ) return 0;
  return (number & (1 << (n_bit-1))) !=0 ? 1 : 0;
}

void find_mode_waveio_and_set_mode_ak4497 ( void )
{
  int i, bits_mode=0;

  // Audio on?
  if ( waveio_pin[PIN_HOST_WAVEIO].mode==1 && waveio_pin[PIN_AUDIO_WAVEIO].mode==1 )
    audio_streaming_waveio=1;
  else
    audio_streaming_waveio=0;

  // calc mask bits mode WaveIO
  for ( i=0; i<COUNT_WAVEIO_PIN_MODE-2; i++ )
    if ( waveio_pin[i].mode )
      bits_mode=set_bit(bits_mode,i+1);

  // convert mask bits mode WaveIO -> current mode audio
  for ( i=0; i<MAX_MODE_PCM_DSD; i++ ) {
    if ( bits_mode==table_convert_modes[i].value_mode_waveio ) {
      ptr_mode_waveio=i;
      break;
    }
  }

  if ( i>=MAX_MODE_PCM_DSD )
    ptr_mode_waveio=NO_MODE; // no found

  log_ln("");
  log_dec("AUDIO",audio_streaming_waveio);
  log_str("MODE",table_convert_modes[ptr_mode_waveio].name_mode);
  log_ln("");

  if ( audio_streaming_waveio )
    ak4497_pin[PIN_MUTE_AK4497].mode=0;
  else
    ak4497_pin[PIN_MUTE_AK4497].mode=1;
  set_pin_mode(&ak4497_pin[PIN_MUTE_AK4497]);

  // mask bits mode ak4497
  bits_mode=table_convert_modes[ptr_mode_waveio].value_mode_ak4497;
  log_bin("BITS",bits_mode);

  // set bit mode ak4497
  for ( i=0; i<COUNT_AK4497_PIN_MODE-1; i++ ) {
    if ( get_bit(bits_mode,i+1) ) 
      ak4497_pin[i].mode=1;
    else
      ak4497_pin[i].mode=0;
    set_pin_mode(&ak4497_pin[i]);
    log_dec(ak4497_pin[i].name_pin,ak4497_pin[i].mode);
  }  

  log_ln("");
}



void setup()
{
  int i;

  #ifdef YES_LOG_SERIAL
  Serial.begin(115200);
  #endif

  log_ln("Start:");
  for ( i=0; i<COUNT_WAVEIO_PIN_MODE; i++ ) {
    init_pin_mode(&waveio_pin[i]);
    get_pin_mode(flag_new_mode_wave_io,&waveio_pin[i]);
    print_pin_mode(&waveio_pin[i]);
  }
  
  for ( i=0; i<COUNT_AK4497_PIN_MODE; i++ ) {
    init_pin_mode(&ak4497_pin[i]);
    set_pin_mode(&ak4497_pin[i]);
  }
  find_mode_waveio();
}

void loop()
{
  int i, count_mode=0, ptr_mode_pcm_dsd=0;

  flag_new_mode_wave_io=0;

  // get pin mode waveio
  for ( i=0; i<COUNT_WAVEIO_PIN_MODE; i++ )
    get_pin_mode(flag_new_mode_wave_io,&waveio_pin[i]);

  // set mode ak4497
  if ( flag_new_mode_wave_io ) {
    for ( i=0; i<COUNT_WAVEIO_PIN_MODE; i++ )
      print_pin_mode(&waveio_pin[i]);
    find_mode_waveio_and_set_mode_ak4497();
  }
  
  //delay(100);
}
