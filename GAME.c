#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

#define MAX_POKEMON_TYPES 3
#define MAX_POKEMON_INFO 100
#define MAX_POKEDEX_SIZE 11
#define MAX_ITEMS 10

// 포켓몬 구조체
typedef struct
{
  // 도감 정보
  char name[15];
  char type[MAX_POKEMON_TYPES][MAX_POKEMON_INFO];
  int minAttack;
  int minHp;

  // 개체 정보
  char nickname[15];
  int attack;
  int maxHp;
  int hp;
} Pokemon;

// 가방 구조체
typedef struct
{
  int monsterball;
  int medicine;
} Bag;

// 트레이너 구조체
typedef struct
{
  int wallet;
  Pokemon entry[6];
  int entry_size;
  int pokedex_size;
  Bag *bag;
  Pokemon *pokemon; // 현재 사용하는 포켓몬
  Pokemon *pokedex;
} Trainer;

// 함수 선언
void savedPokemonInfo(char *filename, Trainer *trainer);
void start(Trainer *, int restart);
void mainGameMenu(Trainer *trainer);
void adventure(Trainer *trainer);
void saveGame(Trainer *trainer);
void store(Trainer *trainer);
void pokemonCenter(Trainer *trainer);
void loadPokemonDict(Trainer *trainer);
void wildPokemon(Trainer *trainer, Pokemon *wildPokemon);
int battle(Trainer *trainer, Pokemon *wildPokemon, int turn);
int openBag(Trainer *trainer, Pokemon *wildPokemoninfo);
int runaway(Trainer *trainer, Pokemon *wildPokemoninfo);
int useMonsterball(Trainer *trainer, Pokemon *wildPokemon);
int goPokemon(Trainer *trainer);

int main()
{
  // 구조체 배열
  Trainer *trainer = (Trainer *)malloc(sizeof(Trainer));
  Pokemon *pokedex = (Pokemon *)malloc(MAX_POKEDEX_SIZE * sizeof(Pokemon));
  Bag *bag = (Bag *)malloc(sizeof(Bag));

  if (trainer == NULL || pokedex == NULL || bag == NULL)
  {
    printf("메모리 할당 오류\n");
    exit(1);
  }

  trainer->pokedex = pokedex;
  trainer->bag = bag;

  // 포켓몬 도감 로딩
  savedPokemonInfo("pokemon.txt", trainer);

  // 게임 시작 화면
  start(trainer, 0);

  // 메모리 해제
  free(trainer->pokedex);
  free(trainer->bag);
  free(trainer);

  return 0;
}

// 포켓몬 정보 구조체에 저장
void savedPokemonInfo(char *filename, Trainer *trainer)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    printf("ERROR: failed %s\n", filename);
    exit(1);
  }

  // 파일의 첫 번째 줄에 포켓몬 수 읽기
  fscanf(file, "%d", &trainer->pokedex_size);

  // 포켓몬 정보 읽어오기
  for (int i = 0; i < trainer->pokedex_size; i++)
  {
    Pokemon new_pokemon;
    fscanf(file, "%s %s %d %d", new_pokemon.name, new_pokemon.type[0], &new_pokemon.minAttack, &new_pokemon.minHp);
    trainer->pokedex[i] = new_pokemon;
  }
  fclose(file);
}

// 시작 화면을 출력
void start(Trainer *trainer, int restart)
{
  printf("===============================\n");
  printf(" K.Knock Pokemon Game\n\n");
  printf(" press enter to start\n");
  printf("===============================\n");
  getchar(); // 사용자가 엔터 키를 누를 때까지 대기

  char ch;
  FILE *welcomefile = fopen("welcomToPokemonWorld.txt", "r");
  while ((ch = fgetc(welcomefile)) != EOF)
  {
    printf("%c", ch);
  }

  fclose(welcomefile);

  // 사용자 선택에 따른 동작 수행
  char choice = '0';
  if (restart == 'y')
  {
    choice = '1';
  }

  // 세이브 파일이 있는지 체크
  FILE *fp = fopen("savefile.txt", "r");
  if (fp == NULL)
  {
    choice = '1'; // 세이브가 없음
  }
  else
  {
    fclose(fp);
  }

  while (1)
  {
    if (choice != '1')
    {
      printf("\n\n   1. 새로하기\t2. 이어하기\n>> ");
      scanf("%c", &choice);
    }

  start:
    if (choice == '1')
    {// 새로하기
      printf("===============================\n");
      // 포켓몬 선택
      printf("어느 포켓몬을 선택하시겠습니까?\n");
      for (int i = 0; i < 3; i++)
      {
        printf("%d. %s\t", i + 1, (trainer->pokedex[i]).name);
      }
      printf("\n>> ");
      int startPokemonIndex;
      scanf("%d", &startPokemonIndex);

      FILE *charmander = fopen("Charmander.txt", "r");
      FILE *bulbasaur = fopen("Bulbasaur.txt", "r");
      FILE *squirtle = fopen("Squirtle.txt", "r");

      if (startPokemonIndex == 1) {
        while ((ch = fgetc(charmander)) != EOF)
        {
          printf("%c", ch);
        }
        printf("파이리 선택!\n");
      }
      else if (startPokemonIndex == 2) {
        while ((ch = fgetc(bulbasaur)) != EOF)
        {
          printf("%c", ch);
        }
        printf("이상해씨 선택!\n");
      }
      else if (startPokemonIndex == 3) {
        while ((ch = fgetc(squirtle)) != EOF)
        {
          printf("%c", ch);
        }
        printf("꼬부기 선택!\n");
      } else {
        printf("잘못된 선택입니다.\n");
        goto start;
      }

      fclose(charmander);
      fclose(bulbasaur);
      fclose(squirtle);

      char inputnickname[15]; // 포켓몬 별명을 저장할 배열
      printf("===============================\n");
      printf("포켓몬의 별명을 입력해주세요.\n");
      scanf("%s", inputnickname); // 포켓몬 별명 입력
      printf("\n");

      // 트레이너 포켓몬 정보 초기화
      Pokemon startPokemonInfo = trainer->pokedex[startPokemonIndex - 1]; // 도감에서 선택한 포켓몬 정보 가져오기
      Pokemon *startPokemon = trainer->entry;                             // 트레이너 포켓몬 정보에 저장

      // 도감정보
      strcpy(startPokemon->name, startPokemonInfo.name);       // 포켓몬 이름 설정
      strcpy(startPokemon->type[0], startPokemonInfo.type[0]); // 포켓몬 속성 설정
      startPokemon->minAttack = startPokemonInfo.minAttack;    // 최소 공격력 설정
      startPokemon->minHp = startPokemonInfo.minHp;            // 최소 체력 설정

      // 개체정보
      strcpy(startPokemon->nickname, inputnickname);                    // 포켓몬 별명 설정
      startPokemon->attack = startPokemonInfo.minAttack + rand() % 101; // 최소 공격력 + 0~100 사이의 랜덤 값 설정
      startPokemon->maxHp = startPokemonInfo.minHp + rand() % 151;      // 최소 체력 + 0~150 사이의 랜덤 값 설정
      startPokemon->hp = startPokemon->maxHp;                           // 현재 체력 = 최대 체력

      // 트레이너 정보 초기화
      trainer->wallet = 10000;
      trainer->bag->monsterball = 0;
      trainer->bag->medicine = 0;
      trainer->entry_size = 1;

      printf("트레이너가 %s(%s)를 선택했습니다!\n", startPokemon->nickname, startPokemon->name);
      printf("게임을 시작합니다.\n");

      break;
    }
    else if (choice == '2')
    {
      FILE *saveFile = fopen("savefile.txt", "r");
      FILE *trainerPokemonFile = fopen("trainerPokemon.txt", "r");
      FILE *bagFile = fopen("bag.txt", "r");
      if (saveFile == NULL || trainerPokemonFile == NULL || bagFile == NULL)
      {
        printf("저장된 여정이 없습니다.\n");
        continue;
      }

      fscanf(trainerPokemonFile, "%d", &trainer->entry_size);
      for (int i = 0; i < trainer->entry_size; i++)
      {
        fscanf(trainerPokemonFile, "%s %s %d %d %s %d %d %d\n", trainer->entry[i].name, trainer->entry[i].type[0], &trainer->entry[i].minAttack, &trainer->entry[i].minHp, trainer->entry[i].nickname, &trainer->entry[i].attack, &trainer->entry[i].maxHp, &trainer->entry[i].hp);
      }
      fscanf(saveFile, "%d", &trainer->wallet);
      fscanf(bagFile, "%d %d", &trainer->bag->monsterball, &trainer->bag->medicine);

      fclose(saveFile);
      fclose(trainerPokemonFile);
      fclose(bagFile);

      break;
    }
  }

  // 사용 할 포켓몬 자동 선택
  for (int i = 0; i < 6; i++)
  {
    if (trainer->entry[i].hp > 0)
    {
      trainer->pokemon = &trainer->entry[i];
      break;
    }
  }
  trainer->pokemon = trainer->entry;

  mainGameMenu(trainer);
}

// 체력이 0 보다 큰 포켓몬 중 내보낼 포켓몬 선택
int goPokemon(Trainer *trainer)
{
  int alive_pokemon = 0;
  for (int i = 0; i < trainer->entry_size; i++)
  {
    if (trainer->entry[i].hp > 0)
    {
      alive_pokemon++;
    }
  }

  if (alive_pokemon == 0)
  {
    printf("내보낼 포켓몬이 없습니다.\n");
    return 0;
  }

  printf("내보낼 포켓몬을 선택해주세요.\n");
  for (int i = 0; i < trainer->entry_size; i++)
  {
    if (trainer->entry[i].hp > 0)
    {
      printf("%d. %s\n", i + 1, trainer->entry[i].nickname);
    }
  }

  int choice;
  while (trainer->pokemon->hp <= 0)
  {
    scanf("%d", &choice);
    if (choice < 1 || choice > trainer->entry_size)
    {
      printf("잘못된 선택입니다.\n");
      continue;
    }
    else if (trainer->entry[choice - 1].hp <= 0)
    {
      printf("%s는 싸울 수 없습니다.\n", trainer->entry[choice - 1].nickname);
      continue;
    }
    trainer->pokemon = &trainer->entry[choice - 1];
    printf("%s를 내보냈습니다.\n", trainer->pokemon->nickname);
    return 1;
  }
}

// 메인 게임
void mainGameMenu(Trainer *trainer)
{
  char select[2];
  do
  {
    printf("===============================\n");
    printf("모험을 진행하시겠습니까?\n");
    printf(" 1. 네\t2. 저장\t3. 상점\t4. 포켓몬센터\t5. 포켓몬 도감\n>> ");
    scanf("%s", select);

    switch (select[0])
    {
    case '1':
      adventure(trainer);
      break;
    case '2':
      saveGame(trainer);
      break;
    case '3':
      store(trainer);
      break;
    case '4':
      pokemonCenter(trainer);
      break;
    case '5':
      loadPokemonDict(trainer);
      break;
    default:
      printf("잘못된 선택입니다.\n");
    }
  } while (select[0] >= '1' && select[0] <= '5');
}

// 모험 진행
void adventure(Trainer *trainer)
{
  printf("===============================\n");
  printf("포켓몬을 탐색하는중 ...\n");
  // 잠시 대기
  srand(time(NULL));
  int wait_time = rand() % 5 + 1; // 1~5 사이의 난수 생성
  Sleep(wait_time * 1000);

  // 야생 포켓몬 생성 및 정보 초기화
  Pokemon *wild = (Pokemon *)malloc(sizeof(Pokemon));
  wildPokemon(trainer, wild);

  printf("===============================\n");
  printf("앗! 야생의 %s가 나타났다!\n", wild->name);
  printf("===============================\n");
  printf("\t\t\t%s\n", wild->name);
  printf("\t\t\t%d/%d\n", wild->hp, wild->maxHp);
  printf("%s\n", trainer->pokemon->nickname);
  printf("%d/%d\n", trainer->pokemon->hp, trainer->pokemon->maxHp);
  printf("===============================\n\n");
  printf("무엇을 해야할까?\n");
  printf("1. 공격 2. 가방 열기 3. 도망치기\n");
  printf(">> ");

  int isBattleOver = 0;
  int adventureChoice;

  while (1)
  {
    scanf("%d", &adventureChoice);

    if (adventureChoice < 1 || adventureChoice > 3)
    {
      printf("잘못된 선택입니다.\n");
      continue;
    }
    else if (adventureChoice == 3)
    {
      isBattleOver = runaway(trainer, wild);
    }
    else if (adventureChoice == 2)
    {
      if (trainer->bag->monsterball == 0 && trainer->bag->medicine == 0)
      {
        printf("가방이 비어있습니다.\n");
      }
      else
      {
        isBattleOver = openBag(trainer, wild);
      }
    }
    else
    {
      isBattleOver = battle(trainer, wild, 0);
    }

    Sleep(wait_time * 1000);

    if (isBattleOver)
    {
      free(wild);
      return isBattleOver;
    }

    printf("\n===============================\n");
    printf("\t\t\t%s\n", wild->name);
    printf("\t\t\t%d/%d\n", wild->hp, wild->maxHp);
    printf("%s\n", trainer->pokemon->nickname);
    printf("%d/%d\n", trainer->pokemon->hp, trainer->pokemon->maxHp);
    printf("===============================\n\n");
    printf("무엇을 해야할까?\n");
    printf("1. 공격 2. 가방 열기 3. 도망치기\n");
    printf(">> ");
  }
}

// 야생 포켓몬 생성
void wildPokemon(Trainer *trainer, Pokemon *wildPokemon)
{
  // 랜덤한 포켓몬 선택
  srand(time(NULL));
  int random_index = rand() % trainer->pokedex_size;
  Pokemon *selectedPokemon = &trainer->pokedex[random_index];

  // 야생 포켓몬 정보 초기화
  // 도감 정보
  strcpy(wildPokemon->name, selectedPokemon->name);
  strcpy(wildPokemon->type[0], selectedPokemon->type[0]);
  wildPokemon->minAttack = selectedPokemon->minAttack;
  wildPokemon->minHp = selectedPokemon->minHp;

  // 개체 정보
  strcpy(wildPokemon->nickname, wildPokemon->name);
  wildPokemon->attack = selectedPokemon->minAttack + rand() % 101; // 최소 공격력 + 0~100 사이의 랜덤 값 설정
  wildPokemon->maxHp = selectedPokemon->minHp + rand() % 151;      // 최소 체력 + 0~150 사이의 랜덤 값 설정
  wildPokemon->hp = wildPokemon->maxHp;                            // 현재 체력 = 최대 체력
}

// 공격
int battle(Trainer *trainer, Pokemon *wildPokemon, int turn) // turn: 0(랜덤 턴), 1(내 턴), -1(야생 포켓몬 턴)
{
  int lastTurn = turn;
  Pokemon *attackPokemon;
  Pokemon *defendPokemon;

  if (!turn)
  {
    turn = rand() % 2 == 0 ? 1 : -1;
  }

  if (turn > 0)
  {
    attackPokemon = trainer->pokemon;
    defendPokemon = wildPokemon;
  }
  else if (turn < 0)
  {
    attackPokemon = wildPokemon;
    defendPokemon = trainer->pokemon;
  }
  
  // 데미지 계산
  int damage = attackPokemon->attack;

  // 공격 수행
  defendPokemon->hp -= damage;

  printf("\n===============================\n");
  printf("%s의 공격!\n", attackPokemon->nickname);
  printf("%s는 %d의 데미지를 입었다.\n", defendPokemon->nickname, damage);

  // 약점과 강점에 따른 데미지 조절
  if (
      strcmp(attackPokemon->type, "풀") == 0 && strcmp(defendPokemon->type, "불") == 0 ||
      strcmp(attackPokemon->type, "불") == 0 && strcmp(defendPokemon->type, "물") == 0 ||
      strcmp(attackPokemon->type, "물") == 0 && strcmp(defendPokemon->type, "풀") == 0)
  {
    damage *= 0.5;
    printf("효과가 별로인 듯 하다.\n");
  }
  else if (
      strcmp(attackPokemon->type, "불") == 0 && strcmp(defendPokemon->type, "풀") == 0 ||
      strcmp(attackPokemon->type, "물") == 0 && strcmp(defendPokemon->type, "불") == 0 ||
      strcmp(attackPokemon->type, "풀") == 0 && strcmp(defendPokemon->type, "물") == 0)
  {
    damage *= 1.5;
    printf("효과가 굉장했다!\n");
  }

  // 크리티컬 확률 적용
  if (rand() % 100 < 20)
  {
    damage *= 1.5;
    printf("급소에 맞았다!\n");
  }
  printf("===============================\n");
  // 포켓몬이 살아있는지 확인
  if (defendPokemon->hp <= 0)
  {
    // 만약 내 포켓몬이 죽었다면
    if (defendPokemon == trainer->pokemon)
    {
      printf("%s가 쓰러졌다.\n", defendPokemon->nickname);

      // 내보낼 포켓몬 선택
      if (!goPokemon(trainer))
      {
        // 내보낼 포켓몬 선택
        printf("모든 포켓몬이 쓰러졌습니다.\n");
        trainer->wallet -= 1000;

        return 3; // 전멸

        getchar(); // 사용자가 엔터 키를 누를 때까지 대기
        printf("(엔터입력 후 메인화면으로)");
        getchar();
      }
    }
    else
    {
      printf("%s를 이겼습니다!\n", defendPokemon->name);
      // 상점에서 포켓몬을 잡아 돈을 얻음 (300~500원)
      int earnedMoney = rand() % 201 + 300; // 300~500 사이의 랜덤 값
      printf("%d원을 얻었습니다.\n", earnedMoney);
      trainer->wallet += earnedMoney;

      return 1; // 승리

      getchar(); // 사용자가 엔터 키를 누를 때까지 대기
      printf("(엔터입력 후 메인화면으로)");
      getchar();
    }
  }

  // 랜덤 턴이었을 경우 상대 턴으로 넘어감
  if (lastTurn == 0)
  {
    battle(trainer, wildPokemon, -turn);
  }

  return 0; // 계속 진행
}

// 가방 열기
int openBag(Trainer *trainer, Pokemon *wildPokemoninfo)
{
  if (trainer->bag->monsterball == 0 && trainer->bag->medicine == 0)
  {
    printf("가방이 비어있습니다.\n");
    return;
  }

  int choice;

  printf("===============================\n");
  printf("가방에서 무엇을 할까?\n");

  // 보유한 가방 물건 리스트 출력
  printf("1. 몬스터볼 (%d 보유)\n", trainer->bag->monsterball);
  printf("2. 상처약 (%d 보유)\n", trainer->bag->medicine);
  printf("0. 뒤로 가기\n");
  printf(">> ");
  scanf("%d", &choice);

  switch (choice)
  {
  case 0:
    // 뒤로 가기
    return;
  case 1:
    // 몬스터볼 사용
    if (trainer->bag->monsterball <= 0)
    {
      printf("몬스터볼이 없습니다.\n");
      break;
    }
    // 몬스터볼 사용 함수 호출
    return useMonsterball(trainer, wildPokemoninfo);
  case 2:
    return useMedicine(trainer, wildPokemoninfo);
  default:
    printf("잘못된 선택입니다.\n");
    break;
  }
}

// 몬스터볼 사용
int useMonsterball(Trainer *trainer, Pokemon *wildPokemoninfo)
{
  // 몬스터볼 사용 효과 출력
  printf("===============================\n");
  printf("몬스터볼을 던졌다!\n");

  // 포획 성공 여부 결정 (랜덤 확률)
  srand(time(NULL));
  int catch_rate = rand() % 101;                                                 // 0~100 사이의 랜덤 값 생성
  int success_rate = 100 - (wildPokemoninfo->hp * 100 / wildPokemoninfo->maxHp); // 포획 성공률 계산

  // 가방에서 몬스터볼 1개 감소
  trainer->bag->monsterball -= 1;

  // 포획 성공 시
  if (catch_rate < success_rate)
  {
    printf("%s를 잡았다!\n", wildPokemoninfo->name);
    printf("별명을 입력하세요 : ");
    char nickname[15];
    scanf("%s", nickname);
    if (nickname[0] != '\n') // 별명이 입력되었을 경우
    {
      strcpy(wildPokemoninfo->nickname, nickname);
    }
    trainer->entry[trainer->entry_size] = *wildPokemoninfo;
    trainer->entry_size += 1;

    // 엔트리에 빈 칸이 있는지 확인
    if (trainer->entry_size == 5)
    {
      printf("포켓몬 마스터가 되었다!.\n");

      // 마지막 상태를 출력 (포켓몬 리스트, 돈, 가방 등)
      printf("포켓몬 리스트\n");
      printf("닉네임(이름) | 속성 | 공격 | 남은 hp |\n");
      for (int i = 0; i < trainer->entry_size; i++)
      {
        printf("%s(%s) %s %d %d\n", trainer->entry[i].nickname, trainer->entry[i].name, trainer->entry[i].type[0], trainer->entry[i].attack, trainer->entry[i].hp);
      }
      printf("돈 : %d\n", trainer->wallet);
      printf("가방 : 몬스터볼 %d개, 상처약 %d개\n", trainer->bag->monsterball, trainer->bag->medicine);

      // 재시작 여부 체크
      printf("게임을 재시작하겠습니까?\n");
      printf("네(Y)\t아니오(N)\n");
      char restart;
      scanf("%c", &restart);
      if (restart == 'y' || restart == 'Y')
      {
        return 2;
      }
      else
      {
        printf("게임을 종료합니다.\n");
        exit(0);
      }
      return;
    }
    else
    {
      return 1;
    }
  }
  // 포획 실패 시
  else
  {
    printf("포켓몬이 몬스터볼에서 빠져나왔다!.\n");
    return battle(trainer, wildPokemoninfo, 0);
  }
}

int useMedicine(Trainer *trainer, Pokemon *wildPokemoninfo)
{
  // 상처약 사용 효과 출력
  printf("===============================\n");
  printf("상처약을 사용했다!\n");

  // 상처약 사용 효과 적용
  trainer->pokemon->hp += trainer->pokemon->maxHp / 100 * 30; // 최대 체력의 30% 회복
  if (trainer->pokemon->hp > trainer->pokemon->maxHp)
  {
    trainer->pokemon->hp = trainer->pokemon->maxHp;
  }

  // 가방에서 상처약 1개 감소
  trainer->bag->medicine -= 1;

  // 상처약 사용 메시지 출력
  printf("%s의 체력이 회복되었다!\n", trainer->pokemon->name);

  return battle(trainer, wildPokemoninfo, 0);
}

// 도망치기
int runaway(Trainer *trainer, Pokemon *wildPokemoninfo)
{
  // 몬스터볼 사용 효과 출력
  printf("===============================\n");
  printf("...!\n");

  // 도망 성공 여부 결정 (랜덤 확률)
  srand(time(NULL));
  int runaway_rate = rand() % 101;
  int runaway_success_rate;                                  // 도망 성공률
  if (wildPokemoninfo->hp == wildPokemoninfo->maxHp) // 체력이 100%일 때
  {
    runaway_success_rate = 10;
  }
  else if (wildPokemoninfo->hp >= wildPokemoninfo->maxHp * 0.6) // 체력이 60% 이상일 때
  {
    runaway_success_rate = 40;
  }
  else if (wildPokemoninfo->hp >= wildPokemoninfo->maxHp * 0.3) // 체력이 40% 이상일 때
  {
    runaway_success_rate = 60;
  }
  else // 체력이 40% 미만일 때
  {
    runaway_success_rate = 90;
  }

  // 도망 성공 시
  if (runaway_rate < runaway_success_rate)
  {
    printf("성공적으로 도망쳤다!\n");
    return 1;
  }

  // 도망 실패 시
  else
  {
    printf("도망치지 못했다!\n");
    return battle(trainer, wildPokemoninfo, 0);
  }
}

// 저장
void saveGame(Trainer *trainer)
{
  FILE *saveFile = fopen("savefile.txt", "w");
  FILE *trainerPokemonFile = fopen("trainerPokemon.txt", "w");
  FILE *bagFile = fopen("bag.txt", "w");
  if (saveFile == NULL || trainerPokemonFile == NULL || bagFile == NULL)
  {
    printf("파일을 열 수 없습니다.\n");
    exit(1);
  }

  // savefile.txt 파일에 트레이너 정보 저장하기 시작
  fprintf(saveFile, "%d\n", trainer->wallet);

  // trainerPokemon.txt 파일에 트레이너 포켓몬 정보 저장하기 시작
  fprintf(trainerPokemonFile, "%d\n", trainer->entry_size);
  for (int i = 0; i < trainer->entry_size; i++)
  {
    fprintf(trainerPokemonFile, "%s %s %d %d %s %d %d %d\n", trainer->entry[i].name, trainer->entry[i].type[0], trainer->entry[i].minAttack, trainer->entry[i].minHp, trainer->entry[i].nickname, trainer->entry[i].attack, trainer->entry[i].maxHp, trainer->entry[i].hp);
  }

  // bag.txt 파일에 가방 정보 저장하기 시작
  fprintf(bagFile, "%d %d\n", trainer->bag->monsterball, trainer->bag->medicine);

  printf("저장되었습니다.\n");
  fclose(saveFile);
  fclose(trainerPokemonFile);
  fclose(bagFile);
}

// 상점
void store(Trainer *trainer)
{
  int itemChoice, amount;

  if (trainer->wallet <= 0)
  {
    printf("돈이 없습니다.\n");
    return;
  }

  while (1)
  {
    printf("===============================\n상점\t\t\t\t\t\t지갑 : %d원\n1. 몬스터볼 1000원\n2. 상처약 500원\n===============================\n무엇을 구매할까? (나가기 0)\n>>", trainer->wallet);
    scanf("%d", &itemChoice);

    char *itemChoiced = itemChoice == 1 ? "몬스터볼" : "상처약";
    int itemChoicedPrice = itemChoice == 1 ? 1000 : 500;

    if (itemChoice < 0 || itemChoice > 2)
    {
      printf("잘못된 선택입니다.\n");
      continue;
    }
    else if (itemChoice == 0)
    {
      return;
    }
    else if (itemChoice == 1 || itemChoice == 2)
    {
      while (1)
      {
        printf("%s을 몇 개 구매할까? (취소 -1)\n>>", itemChoiced);
        scanf("%d", &amount);
        if (amount == -1)
        {
          printf("취소");
          break;
        }
        else if (amount < 1)
        {
          printf("올바른 갯수를 입력해주세요.\n");
          continue;
        }
        else
        {
          // 구매한 아이템의 가격을 계산하여 지갑에서 차감
          int totalCost = amount * itemChoicedPrice;
          if (totalCost > trainer->wallet)
          {
            printf("잔액이 부족합니다.\n");
          }
          else
          {
            trainer->wallet -= totalCost;
            if (itemChoice == 1)
            {
              trainer->bag->monsterball += amount;
            }
            else
            {
              trainer->bag->medicine += amount;
            }
            printf("성공적으로 구매하였다!  (잔액 : %d)\n", trainer->wallet);
            return;
          }
        }
      }
    }
  }
}

// 포켓몬 센터
void pokemonCenter(Trainer *trainer)
{
  printf("===============================\n포켓몬을 치료하는 중입니다");
  Sleep(1000);
  printf(" ...");
  Sleep(1000);
  printf(" ...");
  Sleep(1000);
  printf(" ...\n");

  printf("회복이 완료되었습니다!\n===============================");

  // 엔트리의 모든 포켓몬 체력을 최대 체력으로 설정
  for (int i = 0; i < trainer->entry_size; i++)
  {
    trainer->entry[i].hp = trainer->entry[i].maxHp;
  }

  // 엔트리의 모든 포켓몬을 출력
  printf("포켓몬 리스트\n");
  printf("닉네임(이름) | 속성 | 공격 | 남은 hp |\n");
  for (int i = 0; i < trainer->entry_size; i++)
  {
    printf("%s(%s) %s %d %d\n", trainer->entry[i].nickname, trainer->entry[i].name, trainer->entry[i].type[0], trainer->entry[i].attack, trainer->entry[i].hp);
  }

  getchar(); // 사용자가 엔터 키를 누를 때까지 대기
  printf("(엔터입력 후 메인화면으로)");
  getchar();
}

// 포켓몬 도감
void loadPokemonDict(Trainer *trainer)
{
  printf("포켓몬 도감");
  printf("no| 이름 | 속성 | 최소hp | 최소공격 |\n");
  for (int i = 0; i < trainer->pokedex_size; i++)
  {
    printf("%d | %s | %s | %d | %d |\n", i + 1, trainer->pokedex[i].name, trainer->pokedex[i].type[0], trainer->pokedex[i].minHp, trainer->pokedex[i].minAttack);
  }
}
