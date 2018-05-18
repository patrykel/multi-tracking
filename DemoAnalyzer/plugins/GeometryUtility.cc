

/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors:
 *   Patryk Ławski (patryk.jakub.lawski@cern.ch, patryk.lawski@gmail.com)
 *
 ****************************************************************************/


#include <iostream>
#include "Demo/DemoAnalyzer/interface/GeometryUtility.h"
#include "DataFormats/TotemRPDataTypes/interface/RPRecoHit.h"
#include <vector>
#include <iomanip>


GeometryUtility::Point GeometryUtility::getCenter(RPRecoHit recoHit) {
    unsigned int rawId = recoHit.DetId();

    unsigned int armId = (rawId >> 24) & 0b0001;
    unsigned int stationId = (rawId >> 22) & 0b0011;
    unsigned int rpId = (rawId >> 19) & 0b0111;
    unsigned int siliconId = (rawId >> 15) & 0b1111;

    double x = getX(armId, stationId, rpId, siliconId);
    double y = getY(armId, stationId, rpId, siliconId);
    double z = getZ(armId, stationId, rpId, siliconId);

    GeometryUtility::Point center = {x, y, z};

    return center;
}

GeometryUtility::Direction GeometryUtility::getReadoutDirection(RPRecoHit recoHit) {
    unsigned int rawId = recoHit.DetId();

    unsigned int armId = (rawId >> 24) & 0b0001;
    unsigned int stationId = (rawId >> 22) & 0b0011;
    unsigned int rpId = (rawId >> 19) & 0b0111;
    unsigned int siliconId = (rawId >> 15) & 0b1111;

    double dx = getDx(armId, stationId, rpId, siliconId);
    double dy = getDy(armId, stationId, rpId, siliconId);

    Direction readoutDirection = {dx, dy};

    return readoutDirection;
}

GeometryUtility::Direction GeometryUtility::getPerpendicularDirection(Direction direction) {

    Direction perpendicularDirection = {
            direction.dy,
            -direction.dx
    };

    return perpendicularDirection;
}



/*
OVERVIEW:

 L(x,y)---------T(x,y)              Top             a = 36.07  [mm] (side length)
 |              |                   /\              e = 22.276 [mm] (edge length)
 |              |                 a/  \b            
 |              |                 /    \            
 |       + (0,0)|            Left/      \Right      T_x,  T_y  =  a/2.0,                 a/2.0               
 EL(x,y)        |                \      /           L_x,  L_y  = -a/2.0,                 a/2.0               
  \             |                c\    /d           R_x,  R_y  =  a/2.0,                -a/2.0               
   \            |                  ====             EL_x, EL_y = -a/2.0,                -a/2.0 + e/np.sqrt(2)
    \ER(x,y)----R(x,y)             edge (e)         ER_x, ER_y = -a/2.0 + e/np.sqrt(2), -a/2.0               
*/

/*
Computing intersection of uHit and vHit
  Each hit has position and rawDetId

        c_projection.x = self.rp_silicon.center.x + self.distance * self.rp_silicon.direction.dx
        c_projection.y = self.rp_silicon.center.y + self.distance * self.rp_silicon.direction.dy

*/

/*

Given two lines:
l1:  ( x1 ) + a ( u1)  // linia v
     ( y1 ) + a ( v1)

l2:  ( x2 ) + b ( u2)  // linia u
     ( y2 ) + b ( v2)


[ u1  -u2] * [ a ] = [ x2 - x1 ]
[ v1  -v2]   [ b ]   [ y2 - y1 ]


Cramers rule:

[ a1 b1 ] * [ k_v ] = [ c1 ]
[ a2 b2 ]   [ k_u ]   [ c2 ]

k_v = (c1b2 - b1c2) / (a1b2 - b1a2)
k_u = (a1c2 - c1a2) / (a1b2 - b1a2)

*/

GeometryUtility::PossibleHitPoint GeometryUtility::getIntersection(RPRecoHit uHit, RPRecoHit vHit) {

    // Counting rzut center na kierunek u / v
    
    GeometryUtility::Direction uDirection = this->getReadoutDirection(uHit);
    GeometryUtility::Direction vDirection = this->getReadoutDirection(vHit);

    Point uCenter = getCenter(uHit);
    Point vCenter = getCenter(vHit);

    Point u_p = {
            uCenter.x + uDirection.dx * uHit.Position(),
            uCenter.y + uDirection.dy * uHit.Position()
    };
    Point v_p = {
            vCenter.x + vDirection.dx * vHit.Position(),
            vCenter.y + vDirection.dy * vHit.Position()
    };

    Direction uPerDirection = getPerpendicularDirection(uDirection);
    Direction vPerDirection = getPerpendicularDirection(vDirection);

    // Cramer's rule
    double a1 = vPerDirection.dx;
    double a2 = vPerDirection.dy;
    double b1 = -uPerDirection.dx;
    double b2 = -uPerDirection.dy;
    double c1 = u_p.x - v_p.x;
    double c2 = u_p.y - v_p.y;

    double k_v = (c1 * b2 - b1 * c2) / (a1 * b2 - b1 * a2);

    PossibleHitPoint php = {
            v_p.x + k_v * vPerDirection.dx,   // x
            v_p.y + k_v * vPerDirection.dy,   // y
            (uCenter.z + vCenter.z) / 2.0     // z 
    };

    return php;
}

/*
Iterate over pairs of u/v lines
  Each line has related hits
    Take hit from defined uSiliconNo and vSiliconNo
      Compute intersection
*/
void GeometryUtility::getPossibleHitPoints(std::vector <RPRecognizedPatterns::Line> uLines,
                          std::vector <RPRecognizedPatterns::Line> vLines,
                          unsigned int uSiliconNo,
                          unsigned int vSiliconNo,
                          std::vector <GeometryUtility::PossibleHitPoint> &possibleHits,
                          std::ostringstream &oss) {

    oss << "U/V Intersections from planes:\t" << uSiliconNo << "\t" << vSiliconNo << "\n";
    oss << "\tuLineNo\tvLineNo\t\tx\t\ty\t\tz" << "\n";

    // Iteration for each u and v lines pair
    for (unsigned int ul_i = 0; ul_i < uLines.size(); ul_i++) {
        for (unsigned int vl_i = 0; vl_i < vLines.size(); vl_i++) {

            RPRecoHit uHit;
            RPRecoHit vHit;

            // Taking only hits from selected planes :)
            for (unsigned int uh_i = 0; uh_i < uLines[ul_i].hits.size(); uh_i++) {

                // Take uHit and chceck if is it one we are looking for
                uHit = uLines[ul_i].hits[uh_i];
                if (((uHit.DetId() >> 15) & 0b1111) != uSiliconNo) {
                    continue;
                }

                for (unsigned int vh_i = 0; vh_i < vLines[vl_i].hits.size(); vh_i++) {
                    vHit = vLines[vl_i].hits[vh_i];
                    if (((vHit.DetId() >> 15) & 0b1111) != vSiliconNo) {
                        continue;
                    }

                    // FINALLY THATS THE PLACE WHERE WE DO VALUABLE STUFF
                    GeometryUtility::PossibleHitPoint php = getIntersection(uHit, vHit);
                    oss << "\t" << ul_i << "\t" << vl_i << "\t\t";
                    oss << std::setprecision(7) << std::setw(10) << php.x << "\t";
                    oss << std::setprecision(7) << std::setw(10) << php.y << "\t";
                    oss << std::setprecision(7) << std::setw(10) << php.z << "\n";
                    possibleHits.push_back(php);
                }
            }
        }
        oss << "\n";
    }
}


void GeometryUtility::getPossibleHitPoint(RPRecognizedPatterns::Line uLine,
                                          RPRecognizedPatterns::Line vLines,
                                          std::vector <GeometryUtility::PossibleHitPoint> &possibleHits,
                                          std::ostringstream &oss) {

    oss << "Possible hits from intersection of uLine and vLines:" << "\n";
    oss << "\t" << "uHitNo\tvHitNo\tx\ty\tz" << "\n";

    for (unsigned int uh_i = 0; uh_i < uLine.hits.size(); uh_i++) {
        for (unsigned int vh_i = 0; vh_i < vLines.hits.size(); vh_i++) {
            // Pobierz paski u i v
            RPRecoHit uHit = uLine.hits[uh_i];
            RPRecoHit vHit = vLines.hits[vh_i];

            PossibleHitPoint php = getIntersection(uHit, vHit);
            oss << "\t" << uh_i << "\t" << vh_i << "\t" << php.x << "\t" << php.y << "\t" << php.z << "\n";

            possibleHits.push_back(php);
        }
        oss << "\n";
    }
}


double GeometryUtility::getX(int armId, int stationId, int rpId, int siliconId) {
    return this->x[getIdx(armId, stationId, rpId, siliconId)];
}

double GeometryUtility::getY(int armId, int stationId, int rpId, int siliconId) {
    return this->y[getIdx(armId, stationId, rpId, siliconId)];
}

double GeometryUtility::getZ(int armId, int stationId, int rpId, int siliconId) {
    return this->z[getIdx(armId, stationId, rpId, siliconId)];
}

double GeometryUtility::getDx(int armId, int stationId, int rpId, int siliconId) {
    return this->dx[getIdx(armId, stationId, rpId, siliconId)];
}

double GeometryUtility::getDy(int armId, int stationId, int rpId, int siliconId) {
    return this->dy[getIdx(armId, stationId, rpId, siliconId)];
}


int GeometryUtility::getIdx(int armId, int stationId, int rpId, int siliconId) {
    stationId = stationId == 2 ? 1 : 0;

    int idx = 0;
    idx += armId * station_per_arm * rp_per_station * silicons_per_rp;
    idx += stationId * rp_per_station * silicons_per_rp;
    idx += rpId * silicons_per_rp;
    idx += siliconId;
    return idx;
}
