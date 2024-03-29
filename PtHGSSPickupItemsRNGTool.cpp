#include <iostream>
#include <array>
#include <string_view>
#include <regex>
#include <math.h>
#include <span>

using namespace std;

template <typename T>
void sanitizeInput(const string output, T &index, const T lowLimit, const T highLimit) {
    while ((cout << output) && (!(cin >> index) || (index < lowLimit || index > highLimit))) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void getPickupsNumberInput(short &number) {
    sanitizeInput<short>("Insert the number of Pickup Pokemon in party: ", number, 1, 6);
}

bool sanitizeYesNoInput(const string output) {
    string yesNoAnswer;
    regex hexRegex("^[nNyY]$");

    while ((cout << output) && (!(cin >> yesNoAnswer) || !regex_match(yesNoAnswer, hexRegex))) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    return toupper(yesNoAnswer[0]) == 'Y';
}

bool getSpecificItemFlag() {
    return sanitizeYesNoInput("Specific item slot? (y/n) ");
}

void getLeadMoveRateInput(short &moveRate) {
    cout << "\nMove rates:\n\n1) Infallible hit rate\n2) HM move\n3) 1\%-100\% hit rate\n\n";
    short rateIndex;
    sanitizeInput<short>("Insert the killing move rate of your lead: ", rateIndex, 1, 3);
    moveRate = rateIndex == 1 ? 14 : rateIndex == 2 ? 15 : 16;
}

void getPickupLevelInput(short &level) {
    sanitizeInput<short>("Insert the level of the Pickup Pokemon: ", level, 1, 100);
    cout << "\n\n";
}

short sanitizeGameVersionInput() {
    short gameVersion;
    sanitizeInput<short>("Is the game version Pt (1) or HGSS (2)? ", gameVersion, 1, 2);

    return gameVersion == 2 ? 3 : 1;
}

void printItemsNames(const short level, const short game) {
    static constexpr array pickupNormalPtNames{ to_array<string_view>({ "Potion", "Antidote", "Super Potion", "Great Ball",
                                                                        "Repel", "Escape Rope", "Full Heal", "Hyper Potion",
                                                                        "Ultra Ball", "Revive", "Rare Candy", "Dusk Stone",
                                                                        "Shiny Stone", "Dawn Stone", "Full Restore",
                                                                        "Max Revive", "PP Up", "Max Elixir"}) };

    static constexpr array pickupRarePtNames{ to_array<string_view>({ "Hyper Potion", "Nugget", "King's Rock",
                                                                      "Full Restore", "Ether", "White Herb", "TM44 (Rest)",
                                                                      "Elixir", "TM01 (Focus Punch)", "Leftovers", "TM26 (Earthquake)"}) };

    static constexpr array pickupNormalHGSSNames{ to_array<string_view>({ "Potion", "Antidote", "Super Potion", "Great Ball",
                                                                          "Repel", "Escape Rope", "Full Heal", "Hyper Potion",
                                                                          "Ultra Ball", "Revive", "Rare Candy", "Sun Stone",
                                                                          "Moon Stone", "Heart Scale", "Full Restore",
                                                                          "Max Revive", "PP Up", "Max Elixir"}) };

    static constexpr array pickupRareHGSSNames{ to_array<string_view>({ "Hyper Potion", "Nugget", "King's Rock",
                                                                        "Full Restore", "Ether", "Iron Ball", "TM56 (Fling)",
                                                                        "Elixir", "TM86 (Grass Knot)", "Leftovers", "TM26 (Earthquake)"}) };

    using strview_span = span<const string_view>;

    static constexpr array pickupNames{ to_array<strview_span>({
        strview_span(pickupNormalPtNames),
        strview_span(pickupRarePtNames),
        strview_span(pickupNormalHGSSNames),
        strview_span(pickupRareHGSSNames)
    }) };

    printf("Pickup level %d - %d items:\n\n", ((level == 100 ? 99 : level) / 10) * 10 + 1, ((level == 100 ? 99 : level) / 10) * 10 + 10);

    for (short i = 0; i < 11; i++) {
        if (i < 9) {
            cout << i + 1 << " " << pickupNames[game - 1][((level - 1) / 10) + i] << "\n";
            continue;
        }

        cout << i + 1 << " " << pickupNames[game][((level - 1) / 10) + (10 - i)] << "\n";
    }

    cout << "\n\n";
}

short getItemInput() {
    short item;
    sanitizeInput<short>("Insert the wanted item number: ", item, 1, 11);

    return item;
}

bool getSpecificSlotsFlag() {
    return sanitizeYesNoInput("Search an item slots range? (y/n) ");
}

short getMinSlotInput() {
    short slot;
    sanitizeInput<short>("Insert the wanted lower slot number: ", slot, 1, 11);

    return slot;
}

short getMaxSlotInput() {
    short slot;
    sanitizeInput<short>("Insert the wanted higher slot number: ", slot, 1, 11);

    return slot;
}

bool getInfInfiniteResearchFlag() {
    return sanitizeYesNoInput("\nInfinite research? (y/n) ");
}

uint32_t LCRNG(uint32_t seed) {
    return 0x41C64E6D * seed + 0x6073;
}

uint32_t advanceRNG(uint32_t &seed, unsigned long n = 1) {
    for (unsigned long i = 0; i < n; i++) {
        seed = LCRNG(seed);
    }

    return seed;
}

bool pickupCheck(uint32_t &seed) {
    return (advanceRNG(seed) >> 16) % 10 == 0;
}

short isWantedItemCheck(uint32_t &seed, short item, short minSlot) {
    advanceRNG(seed);
    static constexpr array<short, 11> pickupThresholds{ 0, 30, 40, 50, 60, 70, 80, 90, 94, 98, 99 };
    short pickupValue = (seed >> 16) % 100;

    if (!item) { // Search the slot of the random item
        minSlot = 1;
        item = 11;
    }

    for (short i = minSlot ? minSlot : item; i <= item; i++) {
        if (i > 9) {
            if (pickupValue == pickupThresholds[10 - (11 - i)]) {
                return 11 - (11 - i);
            }

            continue;
        }

        if (pickupValue >= pickupThresholds[i - 1] && pickupValue < pickupThresholds[i]) {
            return i;
        }
    }

    return 0;
}

void findPickupSeed(bool infinite, short pickups, short moveRate, array<short, 6> items, array<short, 6> minSlots) {
    short hour = infinite ? 256 : 24;
    uint32_t minDelay = 600, maxDelay = infinite ? 0x10000 : 10000;

    cout << "\n\n";

    for (short ab = 0; ab < 256; ab++) {
        for (short cd = 0; cd < hour; cd++) {
            for (uint32_t efgh = minDelay; efgh < maxDelay; efgh++) {
                uint32_t seed = ((ab << 24) | (cd << 16)) + efgh;
                uint32_t tempSeed = seed;
                advanceRNG(tempSeed, moveRate); // 14 advances for --% move, 15 advances if it is an HM move, 16 advances if it is a 1-100% move
                short counter = 0;
                array<short, 6> itemSlots = { 0, 0, 0, 0, 0, 0 };

                for (short i = 0; i < pickups; i++) {
                    short itemSlot;

                    if (!pickupCheck(tempSeed) || !(itemSlot = isWantedItemCheck(tempSeed, items[i], minSlots[i]))) {
                        break;
                    }

                    itemSlots[i] = itemSlot;
                    counter++;
                }

                if (counter == pickups) {
                    printf("Initial battle seed: %08X\n", seed);
                    cout << "Items slots: ";

                    for (short i = 0; i < pickups; i++) {
                        cout << itemSlots[i] << (i == pickups - 1 ? "\n" : ", ");
                    }

                    cout << "\n------------------------------------------------\n\n";

                    return;
                }
            }
        }
    }

    printf("No seed found\n\n------------------------------------------------\n\n");

    return;
}

int main() {
    short pickupsNumber, pickupLevel, leadMoveRate;
    uint32_t currentSeed;
    unsigned long currentAdvances, advances;

    while (true) {
        const short gameVersionIndex = sanitizeGameVersionInput();
        getPickupsNumberInput(pickupsNumber);
        getLeadMoveRateInput(leadMoveRate);
        array<short, 6> pickupItemIndexes{ 0, 0, 0, 0, 0, 0 };
        array<short, 6> pickupMinItemIndexes{ 0, 0, 0, 0, 0, 0 };

        for (short i = 0; i < pickupsNumber; i++) {
            static array<string_view, 4> suffixes { "st", "nd", "rd", "th" };
            cout << "\n" << i + 1 << suffixes[(i + 1 > 0 && i + 1 <= 3) ? i : 3] << " Pickup:\n";

            if (getSpecificSlotsFlag()) {
                pickupMinItemIndexes[i] = getMinSlotInput();
                pickupItemIndexes[i] = getMaxSlotInput();
            }
            else if (getSpecificItemFlag()) {
                getPickupLevelInput(pickupLevel);
                printItemsNames(pickupLevel, gameVersionIndex);
                pickupItemIndexes[i] = getItemInput();
            }
        }

        findPickupSeed(getInfInfiniteResearchFlag(), pickupsNumber, leadMoveRate, pickupItemIndexes, pickupMinItemIndexes);
    }

    return 0;
}