#include <common.h>



int main() {

	// Screen buffer
	wchar_t* screen = new wchar_t[ScreenWidth * ScreenHeight];
	HANDLE Console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(Console);
	DWORD BytesWritten = 0;
	
	// Map
	std::wstring map;

	map += L"################";
	map += L"#.....###......#";
	map += L"#..............#";
	map += L"#...#######....#";
	map += L"#.........#....#";
	map += L"#.........#....#";
	map += L"#.....##..#....#";
	map += L"#.....#...#....#";
	map += L"#.....#........#";
	map += L"#..............#";
	map += L"#...#..........#";
	map += L"#...#.....#....#";
	map += L"########..#....#";
	map += L"#.........#....#";
	map += L"#.........###..#";
	map += L"################";


	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();


	// Main loop
	while (1) {

		tp2 = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		double ElapsedTime = elapsedTime.count();

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			PlayerA -= 0.9 * ElapsedTime;
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			PlayerA += 0.9 * ElapsedTime;

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
			PlayerX += sinf(PlayerA) * 5.0 * ElapsedTime;
			PlayerY += cosf(PlayerA) * 5.0 * ElapsedTime;

			if (map[(int)PlayerY * MapWidth + (int)PlayerX] == '#') {
				PlayerX -= sinf(PlayerA) * 5.0 * ElapsedTime;
				PlayerY -= cosf(PlayerA) * 5.0 * ElapsedTime;
			}
		}
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
			PlayerX -= sinf(PlayerA) * 5.0 * ElapsedTime;
			PlayerY -= cosf(PlayerA) * 5.0 * ElapsedTime;

			if (map[(int)PlayerY * MapWidth + (int)PlayerX] == '#') {
				PlayerX += sinf(PlayerA) * 5.0 * ElapsedTime;
				PlayerY += cosf(PlayerA) * 5.0 * ElapsedTime;
			}
		}

		for (int x = 0; x < ScreenWidth; x++) {
			double RayAngle = (PlayerA - Fov / 2.0) + ((double)x / (double)ScreenWidth) * Fov;

			double distanceToWall = 0;
			bool hitWall = false;

			double eyeX = sinf(RayAngle);
			double eyeY = cosf(RayAngle);

			while (!hitWall && distanceToWall < Depth) {
				distanceToWall += 0.1;

				int testX = (int)(PlayerX + eyeX * distanceToWall);
				int testY = (int)(PlayerY + eyeY * distanceToWall);

				if (testX < 0 || testX >= MapWidth || testY < 0 || testY >= MapHeight) {
					hitWall = true;
					distanceToWall = Depth;
				}
				else {
					if (map[testY * MapWidth + testX] == '#')
						hitWall = true;
				}
			}

			int ceiling = (double)(ScreenHeight / 2.0) - ScreenHeight / ((double)distanceToWall);
			int floor = ScreenHeight - ceiling;

			short Shade = ' ';
			short fShade = ' ';

			if (distanceToWall <= Depth / 4.0)			Shade = 0x2588;
			else if (distanceToWall < Depth / 3.0)		Shade = 0x2593;
			else if (distanceToWall < Depth / 2.0)		Shade = 0x2592;
			else if (distanceToWall < Depth)			Shade = 0x2591;
			else										Shade = ' ';

			for (int y = 0; y < ScreenHeight; y++) {
				if (y < ceiling) 
					screen[y * ScreenWidth + x] = ' ';
				else if (y > ceiling && y <= floor)
					screen[y * ScreenWidth + x] = Shade;
				else {
					double f = 1.0 - (((double)y - ScreenHeight / 2.0) / ((double)ScreenHeight / 2.0));
					if (f < 0.25)		fShade = '#';
					else if (f < 0.5)	fShade = 'x';
					else if (f < 0.75)	fShade = '.';
					else if (f < 0.9)	fShade = '-';
					else				fShade = ' ';
					screen[y * ScreenWidth + x] = fShade;
				}
			}
		}

		screen[ScreenWidth * ScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(Console, screen, ScreenWidth * ScreenHeight, { 0,0 }, &BytesWritten);
	}

}