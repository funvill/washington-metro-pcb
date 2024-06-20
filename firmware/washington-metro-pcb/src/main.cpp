#include <Arduino.h>
#include <FastLED.h>

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.
#define DATA_PIN D4
#define LED_TYPE WS2811
#define COLOR_ORDER GRB

// How many leds in your strip?
#define NUM_LEDS 100

// Define the array of leds
CRGB leds[NUM_LEDS];

// How many trains are on each line
const int TRAIN_COUNT = 2;
const int BASE_TRAIN_PROGRESS_SPEED = 1000;

// Modes
const int MODE_DEMO = 1;
int g_mode;

// Start the trains at even spacing across their respective lines
const int lineRedStationCount = 27;
static int lineRedOffset[TRAIN_COUNT];

const int lineOrangeStationCount = 26;
static int lineOrangeOffset[TRAIN_COUNT];

const int lineBlueStationCount = 27;
static int lineBlueOffset[TRAIN_COUNT];

const int lineYellowStationCount = 17;
static int lineYellowOffset[TRAIN_COUNT];

const int lineGreenStationCount = 21;
static int lineGreenOffset[TRAIN_COUNT];

const int lineSilverStationCount = 34;
static int lineSilverOffset[TRAIN_COUNT];

// LINE_RED_OFFSET_TO_LED_INDEX
// This array maps the station offset to the LED index
// The station offset is the number of stations from the start of the line
// The LED index is the number of LEDs from the start of the strip

// Red Line
const int LINE_RED_OFFSET_TO_LED_INDEX[] = {
    37, // A15 Shady Grove
    38, // A14 Rockville
    39, // A13 Twinbrook
    40, // A12 White Flint
    41, // A11 Grosvenor-Strathmore
    42, // A10 Medical Center
    43, // A09 Bethesda
    44, // A08 Friendship Heights
    45, // A07 Tenleytown-AU
    46, // A06 Van Ness-UDC
    47, // A05 Cleveland Park
    48, // A04 Woodley Park-Zoo/Adams Morgan
    49, // A03 Dupont Circle
    50, // A02 Farragut North
    13, // A01 Metro Center
    52, // B01 Gallery Place-Chinatown
    72, // B02 Judiciary Square
    71, // B03 Union Station
    70, // ??? NoMa-Gallaudet U
    69, // B04 Rhode Island Ave-Brentwood
    68, // B05 Brookland-CUA
    67, // B06 Fort Totten
    58, // B07 Takoma
    59, // B08 Silver Spring
    60, // B09 Forest Glen
    61, // B10 Wheaton
    62, // B11 Glenmont
};

const int LINE_ORANGE_OFFSET_TO_LED_INDEX[] = {
    0,  // D13 New Carrollton
    1,  // D12 Landover
    2,  // D11 Cheverly
    3,  // D10 Deanwood
    4,  // D09 Minnesota Ave
    5,  // D08 Stadium-Armory
    6,  // D07 Potomac Ave
    7,  // D06 Eastern Market
    8,  // D05 Capitol South
    9,  // D04 Federal Center SW
    10, // D03 L'Enfant Plaza
    11, // D02 Smithsonian
    12, // D01 Federal Triangle
    13, // C01 Metro Center
    14, // C02 McPherson Square
    15, // C03 Farragut West
    16, // C04 Foggy Bottom-GWU
    17, // C05 Rosslyn
    18, // K01 Court House
    19, // K02 Clarendon
    20, // K03 Virginia Square-GMU
    21, // K04 Ballston-MU
    22, // K05 East Falls Church
    23, // K06 West Falls Church-VT/UVA
    24, // K07 Dunn Loring-Merrifield
    25, // K08 Vienna-Fairfax-GMU
};

const int LINE_BLUE_OFFSET_TO_LED_INDEX[] = {
    97, // G05 Largo Town Center
    96, // G04 Morgan Boulevard
    95, // G03 Addison Road-Seat Pleasant
    94, // G02 Capitol Heights
    93, // G01 Benning Road
    5,  // D08 Stadium-Armory
    6,  // D07 Potomac Ave
    7,  // D06 Eastern Market
    8,  // D05 Capitol South
    9,  // D04 Federal Center SW
    10, // D03 L'Enfant Plaza
    11, // D02 Smithsonian
    12, // D01 Federal Triangle
    13, // C01 Metro Center
    14, // C02 McPherson Square
    15, // C03 Farragut West
    16, // C04 Foggy Bottom-GWU
    17, // C05 Rosslyn
    73, // C06 Arlington Cemetery
    74, // C07 Pentagon
    75, // C08 Pentagon City
    76, // C09 Crystal City
    77, // C10 Ronald Reagan Washington National Airport
    78, // C11 Braddock Road
    80, // J01 King St-Old Town
    83, // J02 Van Dorn Street
    84, // J03 Franconia-Springfield
};

const int LINE_YELLOW_OFFSET_TO_LED_INDEX[] = {
    67, // E06 Fort Totten
    57, // E05 Georgia Ave-Petworth
    56, // E04 Columbia Heights
    55, // E03 U Street/African-Amer Civil War Memorial/Cardozo
    54, // E02 Shaw-Howard U
    53, // E01 Mt Vernon Sq 7th St-Convention Center
    52, // B01 Gallery Place-Chinatown
    51, // ??? Archives-Navy Memorial-Penn Quarter
    10, // D03 L'Enfant Plaza
    74, // C07 Pentagon
    75, // C08 Pentagon City
    76, // C09 Crystal City
    77, // C10 Ronald Reagan Washington National Airport
    78, // C11 Braddock Road
    80, // J01 King St-Old Town
    81, // Eisenhower Avenue
    82, // Huntington
};

const int LINE_GREEN_OFFSET_TO_LED_INDEX[] = {
    63, // E10 Greenbelt
    64, // E09 College Park-U of Md
    65, // E08 Hyattsville crossing
    66, // E07 West Hyattsville
    67, // E06 Fort Totten
    57, // E05 Georgia Ave-Petworth
    56, // E04 Columbia Heights
    55, // E03 U Street/African-Amer Civil War Memorial/Cardozo
    54, // E02 Shaw-Howard U
    53, // E01 Mt Vernon Sq 7th St-Convention Center
    52, // B01 Gallery Place-Chinatown
    51, // ??? Archives-Navy Memorial-Penn Quarter
    10, // D03 L'Enfant Plaza
    85, // F04 Waterfront
    86, // F05 Navy Yard-Ballpark
    87, // F06 Anacostia
    88, // F07 Congress Heights
    89, // F08 Southern Avenue
    90, // F09 Naylor Road
    91, // F10 Suitland
    92, // F11 Branch Avenue
};

const int LINE_SILVER_OFFSET_TO_LED_INDEX[] = {
    36, // N12 Ashburn
    35, // N11 Loudoun Gateway
    34, // N10 Washington Dulles International Airport
    33, // N09 Innovation Center
    32, // N08 Herndon
    31, // N07 Restons Town Center
    30, // N06 Wiehle Reston East
    29, // N05 Spring Hill
    28, // N04 Greensboro
    27, // N03 Tysons Corner
    26, // N02 McLean
    22, // K05 East Falls Church
    21, // K04 Ballston-MU
    20, // K03 Virginia Square-GMU
    19, // K02 Clarendon
    18, // K01 Court House
    17, // C05 Rosslyn
    16, // C04 Foggy Bottom-GWU
    15, // C03 Farragut West
    14, // C02 McPherson Square
    13, // C01 Metro Center
    12, // D01 Federal Triangle
    11, // D02 Smithsonian
    10, // D03 L'Enfant Plaza
    9,  // D04 Federal Center SW
    8,  // D05 Capitol South
    7,  // D06 Eastern Market
    6,  // D07 Potomac Ave
    5,  // D08 Stadium-Armory
    93, // G01 Benning Road
    94, // G02 Capitol Heights
    95, // G03 Addison Road-Seat Pleasant
    96, // G04 Morgan Boulevard
    97, // G05 Largo Town Center
};

void SetTrainStartingPosition(const int stationCount, const int trainCount, int *lineRedTrainOffset)
{
  // Set the starting position of the train
  for (int i = 0; i < trainCount; i++)
  {
    lineRedTrainOffset[i] = (stationCount / trainCount) * i;
  }
}

void setup()
{
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS); // GRB ordering is assumed
  FastLED.setBrightness(33);

  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  Serial.println("Washington Metro PCB Version 0.0.1");
  Serial.println("https://github.com/funvill/washington-metro-pcb");

  g_mode = MODE_DEMO;
  // Run startup once
  static int startup = 0;
  if (startup == 0)
  {
    startup = 1;
    SetTrainStartingPosition(lineRedStationCount, TRAIN_COUNT, lineRedOffset);
    SetTrainStartingPosition(lineOrangeStationCount, TRAIN_COUNT, lineOrangeOffset);
    SetTrainStartingPosition(lineBlueStationCount, TRAIN_COUNT, lineBlueOffset);
    SetTrainStartingPosition(lineYellowStationCount, TRAIN_COUNT, lineYellowOffset);
    SetTrainStartingPosition(lineGreenStationCount, TRAIN_COUNT, lineGreenOffset);
  }
}

void ProgressDemoTrain(int *line, int lineStationCount)
{
  for (int i = 0; i < TRAIN_COUNT; i++)
  {
    // If odd number, move in the positive direction
    // if even number, move in the negative direction
    if (i % 2 == 0)
    {
      line[i]++;
    }
    else
    {
      line[i]--;
    }

    if (line[i] >= lineStationCount)
    {
      line[i] = 0;
    }
    else if (line[i] < 0)
    {
      line[i] = lineStationCount;
    }
  }
}

void loop()
{
  if (g_mode == MODE_DEMO)
  {

    EVERY_N_MILLISECONDS_I(DEMO_RED, BASE_TRAIN_PROGRESS_SPEED + 100)
    {
      ProgressDemoTrain(lineRedOffset, lineRedStationCount);
    }
    EVERY_N_MILLISECONDS_I(DEMO_ORANGE, BASE_TRAIN_PROGRESS_SPEED + 150)
    {
      ProgressDemoTrain(lineOrangeOffset, lineOrangeStationCount);
    }
    EVERY_N_MILLISECONDS_I(DEMO_BLUE, BASE_TRAIN_PROGRESS_SPEED + 250)
    {
      ProgressDemoTrain(lineBlueOffset, lineBlueStationCount);
    }
    EVERY_N_MILLISECONDS_I(DEMO_YELLOW, BASE_TRAIN_PROGRESS_SPEED + 550)
    {
      ProgressDemoTrain(lineYellowOffset, lineYellowStationCount);
    }
    EVERY_N_MILLISECONDS_I(DEMO_GREEN, BASE_TRAIN_PROGRESS_SPEED + 550)
    {
      ProgressDemoTrain(lineGreenOffset, lineGreenStationCount);
    }
    EVERY_N_MILLISECONDS_I(DEMO_SILVER, BASE_TRAIN_PROGRESS_SPEED + 150)
    {
      ProgressDemoTrain(lineSilverOffset, lineSilverStationCount);
    }
  }

  EVERY_N_MILLISECONDS_I(FADE, 20)
  {
    fadeToBlackBy(leds, NUM_LEDS, 20);

    // Draw the trains
    for (int i = 0; i < TRAIN_COUNT; i++)
    {
      leds[LINE_SILVER_OFFSET_TO_LED_INDEX[lineSilverOffset[i]]] = CRGB::LightBlue; // Instead of Silver
      leds[LINE_BLUE_OFFSET_TO_LED_INDEX[lineBlueOffset[i]]] = CRGB::Blue;
      leds[LINE_YELLOW_OFFSET_TO_LED_INDEX[lineYellowOffset[i]]] = CRGB::Yellow;
      leds[LINE_GREEN_OFFSET_TO_LED_INDEX[lineGreenOffset[i]]] = CRGB::Green;
      leds[LINE_ORANGE_OFFSET_TO_LED_INDEX[lineOrangeOffset[i]]] = CRGB::Purple; // Instead of Orange because its too close to Yellow
      leds[LINE_RED_OFFSET_TO_LED_INDEX[lineRedOffset[i]]] = CRGB::Red;
    }
  }

  FastLED.show();
}
