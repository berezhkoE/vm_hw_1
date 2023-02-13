#include <iostream>
#include <vector>
#include <set>
#include <chrono>
#include <algorithm>


char arr[1024 * 1024 * 1024] = {};

int time(int H, int S) {
  int tries = 16;
  int loads = 1000000;

  char **p;
  for (int i = (S - 1) * H; i >= 0; i -= H) {
    char *next;
    p = (char **) &arr[i];

    if (i < H) {
      next = &arr[(S - 1) * H];
    } else {
      next = &arr[i - H];
    }

    *p = next;
  }

  int result = 0;
  for (int j = 0; j < tries; ++j) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < loads; ++i) {
      p = (char **) *p;
    }
    auto end = std::chrono::high_resolution_clock::now();

    result += (int) (end - start).count() / tries;
  }

  return (int) result;
}

int main() {
  int Z = 8 * 1024 * 1024;
  int N = 16;

  int H = 16;
  std::vector<std::set<int>> jumps;
//  std::vector<std::vector<int>> times;

//  int movements = 0;
  for (; H * N < Z; H *= 2) {
    int prevTime = -1;
    std::set<int> currJumps;
//    std::vector<int> currTimes;

    for (int S = 1; S <= N; S++) {
      int currTime = time(H, S);
//      currTimes.push_back(currTime);

      if (prevTime != -1 && (double) (currTime - prevTime) / currTime > 0.1) {
        currJumps.insert(S - 1);
      }

      prevTime = currTime;
    }

    bool isNotMovement = !jumps.empty() && jumps.back() == currJumps;
    if (isNotMovement && /*movements > 2*/ H >= 256 * 1024) {
      break;
    }
//    else if (!isNotMovement && !jumps.empty()) {
//      movements++;
//    }

//    movementDetected = !isNotMovement && !jumps.empty();

//    times.push_back(currTimes);
    jumps.push_back(currJumps);
  }

//  for (size_t i = 0; i < N; ++i) {
//    for (auto & time : times) {
//      std::cout << time[i] << ", ";
//    }
//    std::cout << std::endl;
//  }
//
//  std::cout << std::endl;
//  for (auto& i : jumps) {
//    for (auto& jump: i) {
//      std::cout << jump << ", ";
//    }
//    std::cout << std::endl;
//  }

  //
  std::vector<std::pair<int, int>> entities;

  std::set<int> back = jumps.back();
  for (auto jump = jumps.rbegin(); jump != jumps.rend(); ++jump, H /= 2) {
    if (back.empty()) {
      break;
    }

    std::set<int> tmp;
    for (int j: back) {
      if (!jump->contains(j)) {
        entities.emplace_back(H, j);
      } else {
        tmp.insert(j);
      }
    }
    back = tmp;
  }

  std::sort(entities.begin(), entities.end());
  auto &[HS, assoc] = entities.front();

  int cacheLineSize = -1;
  int prevFirstJump = 1025;
  for (int LS = 1; LS <= HS; LS *= 2) {
    int prevTime = -1;
    int firstJump = -1;

    for (int S = 1; S <= 1024; S *= 2) {
      int currTime = time(HS + LS, S + 1);

      if (prevTime != -1 && firstJump == -1 && (double) (currTime - prevTime) / currTime > 0.1) {
        firstJump = S;
      }

      prevTime = currTime;
    }

    if (firstJump > prevFirstJump) {
      cacheLineSize = LS * assoc;
      break;
    }

    prevFirstJump = firstJump;
  }

  std::cout << "size = " << HS * assoc << std::endl;
  std::cout << "line size = " << cacheLineSize << std::endl;
  std::cout << "associativity = " << assoc << std::endl;
}
