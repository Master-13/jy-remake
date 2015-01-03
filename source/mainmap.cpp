//////////////////////////////////////////////////////////////////////////////////////////
// World map, Scene Map, Battle Map operations
//////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "image.h"
#include "video.h"
#include "util.h"
#include "mainmap.h"


static const int kHalfTileWidth = 18;
static const int kHalfTileHeight = 9;


BattleMap theWarMap;
SceneMap theSceneMap;
WorldMap theWorldMap;


//////////////////////////////////////////////////////////////////////////////////////////
// SceneMap
//////////////////////////////////////////////////////////////////////////////////////////

int SceneMap::load(const char *sfname, const char *dfname)
{
    mapData.create(sfname);
    eventData.create(dfname);
	return 0;

wrong:
    return 1;
}


int SceneMap::save(const char *sceneFilename, const char *eventFilename)
{
	int i;
    RWops file;

    file.fromFile(sceneFilename, "w");
    file.write(mapData.ptr(), 1, mapData.size());
    file.close();

    file.fromFile(eventFilename, "w");
    file.write(eventData.ptr(), 1, eventData.size());
    file.close();

	return 0;
}


void SceneMap::unload()
{
    mapData.destroy();
    eventData.destroy();
}


int SceneMap::getS(int id, int x, int y, int level)
{
	size_t s = 0;
	if (id < 0 || id >= kNumMaps || x < 0 || x >= kWidth || 
            y < 0 || y >= kHeight || level < 0 || level >= kNumLevels) {
        DLOG("data out of range! id=%d, x=%d, y=%d, level=%d", id, x, y, level);
		return 0;
	}
	s = kWidth * kHeight * (id * kNumLevels + level) + y * kWidth + x;
    return static_cast<Sint16 *>(mapData.ptr())[s];
}


int SceneMap::setS(int id, int x, int y, int level, int v)
{
	int s;
	short value = (short) v;
	if (id < 0 || id >= kNumMaps || x < 0 || x >= kWidth || 
            y < 0 || y >= kHeight || level < 0 || level >= kNumLevels) {
		DLOG("data out of range! id=%d, x=%d, y=%d, level=%d", id, x, y, level);
		return 0;
	}

	s = kWidth * kHeight * (id * kNumLevels + level) + y * kWidth + x;
    static_cast<Sint16 *>(mapData.ptr())[s] = value;
	return 0;
}


//��D*
int SceneMap::setD(int sceneid, int id, int i, int v)
{
	size_t s = 0;
	if (sceneid < 0 || sceneid >= kNumMaps) {
		DLOG("sceneid=%d, out of range!", sceneid);
		return 0;
	}
	s = kNumEvents * kNumEventArgs * sceneid + id * kNumEventArgs + i;
    static_cast<Sint16 *>(eventData.ptr())[s] = v;
	return 0;
}


//ȡD*
int SceneMap::getD(int sceneid, int id, int i)
{
	size_t s = 0;
	if (sceneid < 0 || sceneid >= kNumMaps) {
		Log(va("JY_GetD(): sceneid=%d out of range!", sceneid));
		return 0;
	}

    Sint16 * const &p = static_cast<Sint16 *>(eventData.ptr());

	s = kNumEvents * kNumEventArgs * sceneid + id * kNumEventArgs + i;

    //return reinterpret_cast<Sint16 *>(eventData.ptr())[s];
    //return static_cast<Sint16 *>(eventData.ptr())[s];
    return p[s];
}


// ���Ƴ�����ͼ
int SceneMap::draw(int sceneid, int x, int y, int xoff, int yoff, int mypic)
{

	//int rangex=Video_GetScreenWidth()/(2*kHalfTileWidth)/2+1+kExtraWidth;
	//int rangey=(Video_GetScreenHeight())/(2*kHalfTileHeight)/2+1;
	int i, j;
	int i1, j1;
	int x1, y1;
	int xx, yy;
	int istart, iend, jstart, jend;
	SDL_Rect rect;

	rect = Video_GetCanvas()->clip_rect;
	//����Video_GetCanvas()�ļ�����ȷ��ѭ����������߻����ٶ�
	istart = (rect.x - Video_GetScreenWidth() / 2) / 
        (2 * kHalfTileWidth) - 1 - kExtraWidth;
	iend = (rect.x + rect.w - Video_GetScreenWidth() / 2) / 
        (2 * kHalfTileWidth) + 1 + kExtraWidth;

	jstart = (rect.y - Video_GetScreenHeight() / 2) / (2 * kHalfTileHeight) - 1;
	jend = (rect.y + rect.h - Video_GetScreenHeight() / 2) / (2 * kHalfTileHeight) + 1;

	Video_FillColor(0, 0, 0, 0, 0);

	for (j = 0; j <= 2 * jend - 2 * jstart + kExtraHeight; j++) {
		for (i = istart; i <= iend; i++) {
			i1 = i + j / 2 + jstart;
			j1 = -i + j / 2 + j % 2 + jstart;


			x1 = kHalfTileWidth * (i1 - j1) + Video_GetScreenWidth() / 2;
			y1 = kHalfTileHeight * (i1 + j1) + Video_GetScreenHeight() / 2;

			xx = x + i1 + xoff;
			yy = y + j1 + yoff;

			if ((xx >= 0) && (xx < kWidth) && (yy >= 0) && (yy < kHeight)) {
				int d0 = JY_GetS(sceneid, xx, yy, 0);
				if (d0 > 0) {
					Image_DrawCachedImage(0, d0, x1, y1, PIC_JUST_DRAW, 0);	//����
				}
			}
		}
	}

	for (j = 0; j <= 2 * jend - 2 * jstart + kExtraHeight; j++) {
		for (i = istart; i <= iend; i++) {
			i1 = i + j / 2 + jstart;
			j1 = -i + j / 2 + j % 2 + jstart;

			x1 = kHalfTileWidth * (i1 - j1) + Video_GetScreenWidth() / 2;
			y1 = kHalfTileHeight * (i1 + j1) + Video_GetScreenHeight() / 2;

			xx = x + i1 + xoff;
			yy = y + j1 + yoff;

			if ((xx >= 0) && (xx < kWidth) && (yy >= 0) && (yy < kHeight)) {
				int d1 = JY_GetS(sceneid, xx, yy, 1);
				int d2 = JY_GetS(sceneid, xx, yy, 2);
				int d3 = JY_GetS(sceneid, xx, yy, 3);
				int d4 = JY_GetS(sceneid, xx, yy, 4);
				int d5 = JY_GetS(sceneid, xx, yy, 5);

				if (d1 > 0) {
					Image_DrawCachedImage(0, d1, x1, y1 - d4, PIC_JUST_DRAW, 0);	//����
				}
				if (d2 > 0) {
					Image_DrawCachedImage(0, d2, x1, y1 - d5, PIC_JUST_DRAW, 0);	//����
				}
				if (d3 >= 0) {
					int picnum = JY_GetD(sceneid, d3, 7);
					if (picnum > 0) {
						Image_DrawCachedImage(0, picnum, x1, y1 - d4, PIC_JUST_DRAW, 0);
					}
				}

				if ((i1 == -xoff) && (j1 == -yoff)) {	//����
					Image_DrawCachedImage(0, mypic * 2, x1, y1 - d4, PIC_JUST_DRAW, 0);
				}
			}
		}
	}

	return 0;
}


SceneMap::SceneMap()
{
}


SceneMap::~SceneMap()
{
    unload();
}

//////////////////////////////////////////////////////////////////////////////////////////
// BattleMap
//////////////////////////////////////////////////////////////////////////////////////////

//����ս����ͼ
// WarIDXfilename/WarGRPfilename ս����ͼidx/grp�ļ���
// mapid ս����ͼ���
// num ս����ͼ���ݲ���   ӦΪ6
//         0�� ��������
//         1�� ����
//         2�� ս����ս�����
//         3�� �ƶ�ʱ��ʾ���ƶ���λ��
//         4�� ����Ч��
//         5�� ս���˶�Ӧ����ͼ
// x_max,x_max   ��ͼ��С
int BattleMap::load(const char *idxFilename, const char *grpFilename, int mapid)
{
    unload();

	int p;

    RWops file;

	if (pWar == NULL)
		pWar = (Sint16 *) Util_malloc(kWidth * kHeight * kNumLevels * 2);

	if (mapid == 0) {			//��0����ͼ����0��ʼ��
		p = 0;
	} else {
        file.fromFile(idxFilename, "r");
        file.seek(4 * (mapid - 1), RW_SEEK_SET);
        file.read(&p, 4, 1);
        file.close();
	};


    file.fromFile(grpFilename, "r");
    file.seek(p, RW_SEEK_SET);
    file.read(pWar, 2, kWidth * kHeight * 2);

	return 0;
}


void BattleMap::unload()
{
    Util_free(pWar);
}


BattleMap::BattleMap() : pWar(0)
{
}


BattleMap::~BattleMap()
{
    unload();
}

// ����ս����ͼ
// flag=0  ���ƻ���ս����ͼ
//     =1  ��ʾ���ƶ���·����(v1,v2)��ǰ�ƶ����꣬��ɫ����(ѩ��ս��)
//     =2  ��ʾ���ƶ���·����(v1,v2)��ǰ�ƶ����꣬��ɫ����
//     =3  ���е������ð�ɫ������ʾ
//     =4  ս����������  v1 ս������pic, v2��ͼ�����ļ����ļ�id
//                       v3 �书Ч��pic  -1��ʾû���书Ч��

int BattleMap::draw(int flag, int x, int y, int v1, int v2, int v3)
{
	int i, j;
	int i1, j1;
	int x1, y1;
	int xx, yy;
	int istart, iend, jstart, jend;
	SDL_Rect rect;

	rect = Video_GetCanvas()->clip_rect;
	istart = (rect.x - Video_GetScreenWidth() / 2) / (2 * kHalfTileWidth) - 1 - kExtraWidth;
	iend = (rect.x + rect.w - Video_GetScreenWidth() / 2) / (2 * kHalfTileWidth) + 1 + kExtraWidth;
	jstart = (rect.y - Video_GetScreenHeight() / 2) / (2 * kHalfTileHeight) - 1;
	jend = (rect.y + rect.h - Video_GetScreenHeight() / 2) / (2 * kHalfTileHeight) + 1;
	Video_FillColor(0, 0, 0, 0, 0);

	// ��ս������
	for (j = 0; j <= 2 * jend - 2 * jstart + kExtraHeight; j++) {
		for (i = istart; i <= iend; i++) {
			i1 = i + j / 2 + jstart;
			j1 = -i + j / 2 + j % 2 + jstart;

			x1 = kHalfTileWidth * (i1 - j1) + Video_GetScreenWidth() / 2;
			y1 = kHalfTileHeight * (i1 + j1) + Video_GetScreenHeight() / 2;
			xx = x + i1;
			yy = y + j1;
			if ((xx >= 0) && (xx < kWidth) && (yy >= 0) && (yy < kHeight)) {
				int num = JY_GetWarMap(xx, yy, 0);
				if (num > 0)
					Image_DrawCachedImage(0, num, x1, y1, PIC_JUST_DRAW, 0);	//����
			}
		}
	}

	if ((flag == 1) || (flag == 2)) {	//�ڵ����ϻ����ƶ���Χ
		for (j = 0; j <= 2 * jend - 2 * jstart + kExtraHeight; j++) {
			for (i = istart; i <= iend; i++) {
				i1 = i + j / 2 + jstart;
				j1 = -i + j / 2 + j % 2 + jstart;

				x1 = kHalfTileWidth * (i1 - j1) + Video_GetScreenWidth() / 2;
				y1 = kHalfTileHeight * (i1 + j1) + Video_GetScreenHeight() / 2;
				xx = x + i1;
				yy = y + j1;
				if ((xx >= 0) && (xx < kWidth) && (yy >= 0) && (yy < kHeight)) {
					if (JY_GetWarMap(xx, yy, 3) < 128) {
						int showflag;

						if (flag == 1)
							showflag = PIC_DO_ALPHA_BLENDING | PIC_BLACK;
						else
							showflag = PIC_DO_ALPHA_BLENDING | PIC_WHITE;

						if ((xx == v1) && (yy == v2))

							Image_DrawCachedImage(0, 0, x1, y1, showflag, 128);
						else
							Image_DrawCachedImage(0, 0, x1, y1, showflag, 64);
					}
				}
			}
		}
	}
	// ��ս����������
	for (j = 0; j <= 2 * jend - 2 * jstart + kExtraHeight; j++) {
		for (i = istart; i <= iend; i++) {
			i1 = i + j / 2 + jstart;
			j1 = -i + j / 2 + j % 2 + jstart;

			x1 = kHalfTileWidth * (i1 - j1) + Video_GetScreenWidth() / 2;
			y1 = kHalfTileHeight * (i1 + j1) + Video_GetScreenHeight() / 2;
			xx = x + i1;
			yy = y + j1;
			if ((xx >= 0) && (xx < kWidth) && (yy >= 0) && (yy < kHeight)) {
				int num = JY_GetWarMap(xx, yy, 1);	//  ����      
				if (num > 0)
					Image_DrawCachedImage(0, num, x1, y1, 0, 0);

				num = JY_GetWarMap(xx, yy, 2);	// ս����
				if (num >= 0) {
					int pic = JY_GetWarMap(xx, yy, 5);	// ����ͼ
					if (pic >= 0) {
						switch (flag) {
						case 0:
						case 1:
						case 2:
						case 5:	//���ﳣ����ʾ
							Image_DrawCachedImage(0, pic, x1, y1, PIC_JUST_DRAW, 0);
							break;
						case 3:
							if (JY_GetWarMap(xx, yy, 4) > 1)	//����
								Image_DrawCachedImage(0, pic, x1, y1, PIC_BLACK | PIC_DO_ALPHA_BLENDING, 255);	//���
							else
								Image_DrawCachedImage(0, pic, x1, y1, PIC_JUST_DRAW, 0);

							break;
						case 4:
							if ((xx == x) && (yy == y)) {
								if (v2 == 0)
									Image_DrawCachedImage(0, pic, x1, y1, PIC_JUST_DRAW, 0);
								else
									Image_DrawCachedImage(v2, v1, x1, y1, PIC_JUST_DRAW, 0);
							} else {
								Image_DrawCachedImage(0, pic, x1, y1, PIC_JUST_DRAW, 0);
							}

							break;
						}
					}
				}

				if (flag == 4 && v3 >= 0) {	//�书Ч��
					int effect = JY_GetWarMap(xx, yy, 4);
					if (effect > 0) {
						Image_DrawCachedImage(3, v3, x1, y1, PIC_JUST_DRAW, 0);
					}
				}
			}
		}
	}
	return 0;
}


int BattleMap::clean(int level, int v)
{
	Sint16 *p = pWar + kWidth * kHeight * level;
	int i;
	for (i = 0; i < kWidth * kHeight; i++) {
		*p = (Sint16) v;
		p++;
	}
	return 0;
}


int BattleMap::set(int x, int y, int level, int v)
{
	size_t s = 0;

	if (x < 0 || x >= kWidth || y < 0 || y >= kHeight || level < 0 || level >= 6) {
		DLOG("data out of range! x=%d, y=%d, level=%d", x, y, level);
		return 0;
	}
	s = kWidth * kHeight * level + y * kWidth + x;
	//*(pWar + s) = (short) v;
    pWar[s] = static_cast<Sint16>(v);
	return 0;
}


int BattleMap::get(int x, int y, int level)
{
	size_t s = 0;

	if (x < 0 || x >= kWidth || y < 0 || y >= kHeight || level < 0 || level >= 6) {
		DLOG("data out of range");
		return 0;
	}
	s = kWidth * kHeight * level + y * kWidth + x;
	//return *(pWar + s);
    return pWar[s];
}


//////////////////////////////////////////////////////////////////////////////////////////
// WorldMap
//////////////////////////////////////////////////////////////////////////////////////////


WorldMap::WorldMap()
{
}


WorldMap::~WorldMap()
{
    unload();
}

// �ͷ�����ͼ����
int JY_UnloadMMap(void)
{
    return theWorldMap.unload();
}


size_t WorldMap::coord2Index(int x, int y)
{
	size_t s = y * kWidth + x;
	return s;
}


int WorldMap::unload()
{
    earthLayerData.destroy();
	surfaceLayerData.destroy();
	buildingLayerData.destroy();
	buildxLayerData.destroy();
	buildyLayerData.destroy();
	return 0;
}


int WorldMap::load(const char *earthname, const char *surfacename, const char *buildingname,
        const char *buildxname, const char *buildyname)
{
    //unload();
    
    if (earthLayerData.ptr() != NULL) {
        DLOG("earthLayerData already loaded.");
    }

    if (earthLayerData.create(earthname) ||
            surfaceLayerData.create(surfacename) ||
            buildingLayerData.create(buildingname) ||
            buildxLayerData.create(buildxname) ||
            buildyLayerData.create(buildyname))
        goto out;
    return 0;

out:
    Log("Error loading MMap.");
    unload();
    return 1;
}


int WorldMap::get(int x, int y, BuildingType flag)
{
	size_t s = 0;
	int v = 0;

	s = coord2Index(x, y);

	switch (flag) {
	case kLayerEarth:
		v = static_cast<Sint16 *>(earthLayerData.ptr())[s];
		break;

	case kLayerSurface:
		v = static_cast<Sint16 *>(surfaceLayerData.ptr())[s];
		break;
	case kLayerBuilding:
		v = static_cast<Sint16 *>(buildingLayerData.ptr())[s];
		break;
	case kLayerBuildx:
		v = static_cast<Sint16 *>(buildxLayerData.ptr())[s];
		break;
	case kLayerBuildy:
		v = static_cast<Sint16 *>(buildyLayerData.ptr())[s];
		break;
	}
	return v;
}


int WorldMap::set(short x, short y, BuildingType flag, short v)
{
	size_t s = 0;

	s = coord2Index(x, y);

	switch (flag) {
	case kLayerEarth:
        static_cast<Sint16 *>(earthLayerData.ptr())[s] = v;
		break;
	case kLayerSurface: static_cast<Sint16 *>(surfaceLayerData.ptr())[s] = v;
		break;
	case kLayerBuilding:
        static_cast<Sint16 *>(buildingLayerData.ptr())[s] = v;
		break;
	case kLayerBuildx:
        static_cast<Sint16 *>(buildxLayerData.ptr())[s] = v;
		break;
	case kLayerBuildy:
        static_cast<Sint16 *>(buildyLayerData.ptr())[s] = v;
		break;
	}
	return 0;
}


// ����ͼ�������� 
// x,y ��������
// mypic ������ͼ���

int WorldMap::sortBuildings(short x, short y, short mypic, int screen_w, int screen_h, 
        int extra_x, int extra_y, int x_scale, int y_scale)
{

	int rangex = screen_w / (2 * x_scale) / 2 + 1 + extra_x;
	int rangey = screen_h / (2 * y_scale) / 2 + 1;

	int range = rangex + rangey + extra_y;

	short bak = (short) JY_GetMMap(x, y, kLayerBuilding);
	short bakx = (short) JY_GetMMap(x, y, kLayerBuildx);
	short baky = (short) JY_GetMMap(x, y, kLayerBuildy);

	int xmin = clamp(x - range, 1, kWidth - 1);
	int xmax = clamp(x + range, 1, kWidth - 1);
	int ymin = clamp(y - range, 1, kHeight - 1);
	int ymax = clamp(y + range, 1, kHeight - 1);

	int i, j, k, m;
	int dy;
	int repeat = 0;
	int p = 0;

	TBuilding tmp_building;

	JY_SetMMap(x, y, kLayerBuilding, (short) (mypic * 2));
	JY_SetMMap(x, y, kLayerBuildx, x);
	JY_SetMMap(x, y, kLayerBuildy, y);

	for (i = xmin; i <= xmax; i++) {
		dy = ymin;
		for (j = ymin; j <= ymax; j++) {
			int ij3 = JY_GetMMap(i, j, kLayerBuildx);
			int ij4 = JY_GetMMap(i, j, kLayerBuildy);
			if ((ij3 != 0) && (ij4 != 0)) {
				repeat = 0;
				for (k = 0; k < p; k++) {
					if ((buildings[k].x == ij3) && (buildings[k].y == ij4)) {
						repeat = 1;
						if (k == p - 1)
							break;

						for (m = j - 1; m >= dy; m--) {
							int im3 = JY_GetMMap(i, m, kLayerBuildx);
							int im4 = JY_GetMMap(i, m, kLayerBuildy);
							if ((im3 != 0) && (im4 != 0)) {
								if ((im3 != ij3) || (im4 != ij4)) {
									if ((im3 != buildings[k].x) || (im4 != buildings[k].y)) {
										tmp_building = buildings[p - 1];
										memmove(&buildings[k + 1], &buildings[k], (p - 2 - k + 1) * sizeof(TBuilding));
										buildings[k] = tmp_building;
									}
								}
							}
						}
						dy = j + 1;
						break;
					}
				}

				if (repeat == 0) {
					buildings[p].x = ij3;
					buildings[p].y = ij4;
					buildings[p].num = JY_GetMMap(buildings[p].x, buildings[p].y, kLayerBuilding);
					p++;
				}
			}
		}
	}

	//buildings_num = p;

	JY_SetMMap(x, y, kLayerBuilding, bak);
	JY_SetMMap(x, y, kLayerBuildx, bakx);
	JY_SetMMap(x, y, kLayerBuildy, baky);

	return p;
}


int WorldMap::draw(int x, int y, int mypic)
{

	int i, j;
	int i1, j1;
	int x1, y1;
	int picnum;
	int istart, iend, jstart, jend;
	SDL_Rect rect;
	int buildings_num = 0;

	rect = Video_GetCanvas()->clip_rect;
	//����Video_GetCanvas()��clip��ȷ��ѭ����������߻����ٶ�
	istart = (rect.x - Video_GetScreenWidth() / 2) / (2 * kHalfTileWidth) - 1 - kExtraWidth;
	iend = (rect.x + rect.w - Video_GetScreenWidth() / 2) / (2 * kHalfTileWidth) + 1 + kExtraWidth;
	jstart = (rect.y - Video_GetScreenHeight() / 2) / (2 * kHalfTileHeight) - 1;
	jend = (rect.y + rect.h - Video_GetScreenHeight() / 2) / (2 * kHalfTileHeight) + 1;



	//��������
	buildings_num = sortBuildings((short) x, (short) y, (short) mypic, Video_GetScreenWidth(), Video_GetScreenHeight(), kExtraWidth, kExtraHeight, kHalfTileWidth, kHalfTileHeight);
	Video_FillColor(0, 0, 0, 0, 0);
	for (j = 0; j <= 2 * jend - 2 * jstart + kExtraHeight; j++) {
		for (i = istart; i <= iend; i++) {
			i1 = i + j / 2 + jstart;
			j1 = -i + j / 2 + j % 2 + jstart;

			x1 = kHalfTileWidth * (i1 - j1) + Video_GetScreenWidth() / 2;
			y1 = kHalfTileHeight * (i1 + j1) + Video_GetScreenHeight() / 2;

			if (((x + i1) >= 0) && ((x + i1) < kWidth) && ((y + j1) >= 0) && ((y + j1) < kHeight)) {
				picnum = JY_GetMMap(x + i1, y + j1, kLayerEarth);
				if (picnum > 0) {
					Image_DrawCachedImage(0, picnum, x1, y1, PIC_JUST_DRAW, 0);
				}
				picnum = JY_GetMMap(x + i1, y + j1, kLayerSurface);
				if (picnum > 0) {
					Image_DrawCachedImage(0, picnum, x1, y1, PIC_JUST_DRAW, 0);
				}
			}
		}
	}

	for (i = 0; i < buildings_num; i++) {
		i1 = buildings[i].x - x;
		j1 = buildings[i].y - y;
		x1 = kHalfTileWidth * (i1 - j1) + Video_GetScreenWidth() / 2;
		y1 = kHalfTileHeight * (i1 + j1) + Video_GetScreenHeight() / 2;
		picnum = buildings[i].num;
		if (picnum > 0) {
			Image_DrawCachedImage(0, picnum, x1, y1, PIC_JUST_DRAW, 0);
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
// functions
//////////////////////////////////////////////////////////////////////////////////////////

//��ȡS*D*
int JY_LoadSMap(const char *sceneFilename, const char *eventFilename)
{
    JY_UnloadSMap();
    return theSceneMap.load(sceneFilename, eventFilename);
}


//����S*D*
int JY_SaveSMap(const char *sceneFilename, const char *eventFilename)
{
    return theSceneMap.save(sceneFilename, eventFilename);
}


int JY_UnloadSMap()
{
    theSceneMap.unload();
    return 0;
}


//����ʱ�ļ��ж�ȡ����id��S���ݵ��ڴ�
//ȡs��ֵ
int JY_GetS(int id, int x, int y, int level)
{
    return theSceneMap.getS(id, x, y, level);

}


//��S��ֵ
int JY_SetS(int id, int x, int y, int level, int v)
{
    return theSceneMap.setS(id, x, y, level, v);

}


//ȡD*
int JY_GetD(int sceneid, int id, int i)
{
    return theSceneMap.getD(sceneid, id, i);
}


//��D*
int JY_SetD(int sceneid, int id, int i, int v)
{
    return theSceneMap.setD(sceneid, id, i, v);
}


// ���Ƴ�����ͼ
int JY_DrawSMap(int sceneid, int x, int y, int xoff, int yoff, int mypic)
{
    return theSceneMap.draw(sceneid, x, y, xoff, yoff, mypic);

}


int JY_UnloadWarMap()
{
    theWarMap.unload();
    return 0;
}


//����ս����ͼ
// WarIDXfilename/WarGRPfilename ս����ͼidx/grp�ļ���
// mapid ս����ͼ���
// num ս����ͼ���ݲ���   ӦΪ6
//         0�� ��������
//         1�� ����
//         2�� ս����ս�����
//         3�� �ƶ�ʱ��ʾ���ƶ���λ��
//         4�� ����Ч��
//         5�� ս���˶�Ӧ����ͼ
// x_max,x_max   ��ͼ��С
int JY_LoadWarMap(const char *warIdxFilename, const char *warGrpFilename, int mapid)
{
    JY_UnloadWarMap();
    theWarMap.load(warIdxFilename, warGrpFilename, mapid);
    return 0;
}


//ȡս����ͼ����
int JY_GetWarMap(int x, int y, int level)
{
    return theWarMap.get(x, y, level);
}


//��ս����ͼ����
int JY_SetWarMap(int x, int y, int level, int v)
{
    theWarMap.set(x, y, level, v);
    return 0;
}


//����ĳ��ս����ͼΪ����ֵ
int JY_CleanWarMap(int level, int v)
{
    theWarMap.clean(level, v);
    return 0;
}

// ����ս����ͼ
// flag=0  ���ƻ���ս����ͼ
//     =1  ��ʾ���ƶ���·����(v1,v2)��ǰ�ƶ����꣬��ɫ����(ѩ��ս��)
//     =2  ��ʾ���ƶ���·����(v1,v2)��ǰ�ƶ����꣬��ɫ����
//     =3  ���е������ð�ɫ������ʾ
//     =4  ս����������  v1 ս������pic, v2��ͼ�����ļ����ļ�id
//                       v3 �书Ч��pic  -1��ʾû���书Ч��

int JY_DrawWarMap(int flag, int x, int y, int v1, int v2, int v3)
{
    theWarMap.draw(flag, x, y, v1, v2, v3);
    return 0;
}


// ��ȡ����ͼ����
int JY_LoadMMap(const char *earthname, const char *surfacename, const char *buildingname, 
        const char *buildxname, const char *buildyname)
{
    JY_UnloadMMap();
    theWorldMap.load(earthname, surfacename, buildingname, buildxname, buildyname);
}


// ȡ����ͼ���� 
// flag  0 earthLayerData, 1 surfaceLayerData, 2 buildings, 3 buildxLayerData, 4 buildyLayerData
int JY_GetMMap(int x, int y, BuildingType flag)
{
    return theWorldMap.get(x, y, flag);
}


// ������ͼ���� 
// flag  0 earthLayerData, 1 surfaceLayerData, 2 buildings, 3 buildxLayerData, 4 buildyLayerData
int JY_SetMMap(short x, short y, BuildingType flag, short v)
{
    return theWorldMap.set(x, y, flag, v);
}


// ��������ͼ
int JY_DrawMMap(int x, int y, int mypic)
{
    return theWorldMap.draw(x, y, mypic);
}


