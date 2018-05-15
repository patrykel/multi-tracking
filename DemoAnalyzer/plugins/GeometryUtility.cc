

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

void GeometryUtility::getPossibleHitPoint(RPRecognizedPatterns::Line uLine, RPRecognizedPatterns::Line vLine, std::vector<GeometryUtility::PossibleHitPoint> &possibleHits, std::ostringstream &oss){
  // need data:
  //  u [arm | station | rp | silicon]
  //  v [arm | station | rp | silicon]

  // nie ma sensu od razu szukac tych punktow przeciecia --> po prostu wyznacz punkty P ;)
  
  // sciezka --> wez center z punktu
  /*
  sciezka 
    - center
    - P
    - punkt przeciecia i bedzie gitara :)

  */

  // wez po prostu 
  // iteruj po wszystkich hitach z danej uLine i vLine

  // DataFormats/TotemRPDetId/interface/TotRPDetId.h

  
    
  for(unsigned int uh_i = 0; uh_i < uLine.hits.size(); uh_i++){
    // Getting detector id
    RPRecoHit curr_hit = uLine.hits[uh_i];
    unsigned int rawId = curr_hit.DetId();

    // Initializing point structure
    unsigned int armId     = (rawId >> 24) & 0b0001;
    unsigned int stationId = (rawId >> 22) & 0b0011;
    unsigned int rpId      = (rawId >> 19) & 0b0111;
    unsigned int siliconId = (rawId >> 15) & 0b1111;

    double x = getX(armId, stationId, rpId, siliconId);
    double y = getY(armId, stationId, rpId, siliconId);
    double z = getZ(armId, stationId, rpId, siliconId);                            

    GeometryUtility::PossibleHitPoint php = { x, y, z };
            
    // Add point to vector
    possibleHits.push_back(php);
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
