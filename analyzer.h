#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <array>

struct ZoneCount {
    std::string zone;
    long long count;
};

struct SlotCount {
    std::string zone;
    int hour;             // 0–23
    long long count;
};

class TripAnalyzer {
public:
    // Parse Trips.csv, skip dirty rows, never crash
    // Main.cpp burayı çağırdığı için ismi ingestFile olmalı ve dosya yolu almalı
    void ingestFile(const std::string& csvPath);


    // Top K zones: count desc, zone asc
    std::vector<ZoneCount> topZones(int k = 10) const;

    // Top K slots: count desc, zone asc, hour asc
    std::vector<SlotCount> topBusySlots(int k = 10) const;

private:
    // Bu alan tripzone sayımızın tutulacağı Map'i ifade etmekte
    std::unordered_map<std::string, long long> m_zoneTotalCounts;

    // bu alan ise tripzone ve time yani zaman bileşenlerini niteleyen mapi ifade etmekte
    std::unordered_map<std::string, std::array<long long, 24>> m_zoneHourlyCounts;
};
