

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




GeometryUtility::Point 
GeometryUtility::getCenter(RPRecoHit recoHit){
  unsigned int rawId = recoHit.DetId();

  unsigned int armId     = (rawId >> 24) & 0b0001;
  unsigned int stationId = (rawId >> 22) & 0b0011;
  unsigned int rpId      = (rawId >> 19) & 0b0111;
  unsigned int siliconId = (rawId >> 15) & 0b1111;

  double x = getX(armId, stationId, rpId, siliconId);
  double y = getY(armId, stationId, rpId, siliconId);
  double z = getZ(armId, stationId, rpId, siliconId); 

  GeometryUtility::Point center = {x, y, z};

  return center;
}

GeometryUtility::Direction 
GeometryUtility::getReadoutDirection(RPRecoHit recoHit){
  unsigned int rawId = recoHit.DetId();

  unsigned int armId     = (rawId >> 24) & 0b0001;
  unsigned int stationId = (rawId >> 22) & 0b0011;
  unsigned int rpId      = (rawId >> 19) & 0b0111;
  unsigned int siliconId = (rawId >> 15) & 0b1111;

  double dx = getDx(armId, stationId, rpId, siliconId);
  double dy = getDy(armId, stationId, rpId, siliconId);

  GeometryUtility::Direction readoutDirection = {dx, dy};

  return readoutDirection;
}

GeometryUtility::Direction 
GeometryUtility::getPerpendicularDirection(GeometryUtility::Direction direction){
  
  GeometryUtility::Direction perpendicularDirection = {
    -direction.dy, 
    direction.dx
  };

  return perpendicularDirection;
}

// GeometryUtility::Vector2D getCenterShift(GeometryUtility::Point c1, GeometryUtility::Point c2){
  
//   GeometryUtility::Vector2D shift = {
//     c2.x - c1.x;
//     c2.y - c1.y;
//   };

//   return shift;
// }





/*

Given two lines:
l1:  ( x1 ) + a ( u1)
     ( y1 ) + a ( v1)

l2:  ( x2 ) + a ( u2)
     ( y2 ) + a ( v2)


[ u1  -u2] * [ a ] = [ x2 - x1 ]
[ v1  -v2]   [ b ]   [ y2 - y1 ]


Cramers rule:

[ a1 b1 ] * [ k_u ] = [ c1 ]
[ a2 b2 ]   [ k_v ]   [ c2 ]

k_u = (c1b2 - b1c2) / (a1b2 - b1a2)
k_v = (a1c2 - c1a2) / (a1b2 - b1a2)

*/


void GeometryUtility::getPossibleHitPoint(RPRecognizedPatterns::Line uLine, RPRecognizedPatterns::Line vLine, std::vector<GeometryUtility::PossibleHitPoint> &possibleHits, std::ostringstream &oss){
  
  oss << "Possible hits from intersection of uLine and vLine:" << "\n";
  oss << "\t" << "uHitNo\tvHitNo\tx\ty\tz" << "\n";

  for(unsigned int uh_i = 0; uh_i < uLine.hits.size(); uh_i++){
    for(unsigned int vh_i = 0; vh_i < vLine.hits.size(); vh_i++){

      // Pobierz paski u i v
      RPRecoHit u_hit = uLine.hits[uh_i];
      RPRecoHit v_hit = vLine.hits[vh_i]; 

      // Counting rzut center na kierunek u / v
      GeometryUtility::Direction uDirection = getReadoutDirection(u_hit);
      GeometryUtility::Direction vDirection = getReadoutDirection(v_hit);

      GeometryUtility::Direction uPerDirection = getPerpendicularDirection(uDirection);     
      GeometryUtility::Direction vPerDirection = getPerpendicularDirection(vDirection);     

      GeometryUtility::Point uCenter = getCenter(u_hit); 
      GeometryUtility::Point vCenter = getCenter(v_hit); 

      GeometryUtility::Point u_p = {
        uCenter.x + uPerDirection.dx * u_hit.Position(),
        uCenter.y + uPerDirection.dy * u_hit.Position()
      };
      GeometryUtility::Point v_p = {
        vCenter.x + vPerDirection.dx * v_hit.Position(),
        vCenter.y + vPerDirection.dy * v_hit.Position()
      };

      // Cramer's rule
      double a1 = uDirection.dx;
      double a2 = -uDirection.dy;
      double b1 = vDirection.dx;
      double b2 = -vDirection.dy;
      double c1 = v_p.x - u_p.x;
      double c2 = v_p.y - u_p.y;

      double k_u = (c1 * b2 - b1 * c2) / (a1 * b2 - b1 * a2);

      // GeometryUtility::Vector2D centerShift = getCenterShift(uCenter, vCenter);

      GeometryUtility::PossibleHitPoint php = {
        u_p.x + k_u * uDirection.dx,    // x
        u_p.y + k_u * uDirection.dy,    // y 
        (uCenter.z + vCenter.z) / 2.0  // z
      };

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
