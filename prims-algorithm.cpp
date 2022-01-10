#include <iostream>
#include <vector>
#include <climits>

using namespace std;

/*
	구현 방식 - 수업시간때 배운 Unsorted sequence 방식을 적용하여 알고리즘을 작성하였습니다.
	
	필수적인 예외처리 방식
	1) [중복오류] Fringe 배열에서 계산처리가 다 끝난 후 가장 작은 값을 고를때 중복이 발생할 경우 -> 배열번호가 가장 작은 것을 선택하게 처리함
	
	작동 방식
	1) 그래프가 무향 그래프이므로 모든 노드를 연결 시켜 주고 ( A->B / B->A ) 그래프에 디자인 비용을 같이 추가해 준다. [ 백터를 사용하여 인접리스트 기반 사용 ]
	2) 모든 그래프를 만드는 과정이 끝났다면, INPUT 으로 [ P x(숫자) ]를 받았다면 그 숫자에서 부텉 Prim 알고리즘을 수행
		2-1) 처리할때 2개의 배열을 사용한다.
			TREE[N] // 각 노드마다 T인지 F인지 체크 -> 초기에는 Unseen이므로 U로 전부 초기화 함
			Fringe[N] // 최소 비용을 저장하는 배열 -> 초기에는 무한의 값(INT_MAX)로 초기화
		2-2) 입력받은 x의 값부터 시작하므로, Tree[x] 의 값을 T로 만든다. 그 후 Tree[x] 의 연결되어 있는 노드들을 전부 F로 만든다.
			(이때 F로 만들때 이미 T의 값이라면, F로 만들지 않는다)
		2-3) 그 후 X 노드의 연결되어 있는 노드의 수만큼 반복하여, TREE가 F일 경우 Fringe[]의 값을 업데이트 하는데 [ T라면 수행하지 않음 ]
			이때, 가장 작은 Fringe의 값을 선택하여 TREE[min(Fringe)]로 선택하여 반복적으로 수행한다 (언제까지? Fringe가 없을때까지 즉, N번 만큼 수행)
			-> 왜 N번 만큼 만 수행할까? -> Tree로 가는 노드는 처음 입력된 값 빼고 1번만 수행되기 때문이다.
		2-4) 그 후 모든 반복이 끝났으면, Fringe[]배열에는 최소 path의 거리만 남아있으므로 전부 sum 한뒤 출력 하면 되고,
			가는 경로의 노드는 따로 path[N] 배열을 만들어서 저장 후 1~N까지 출력 한다.
*/

int N, M, T; // 노드의 갯수, 간선의 갯수, 최소 거리 비용의 테스트 케이스 수

struct Node {
	int child; // 조형물 2 번호
	int price; // 산책로 디자인 비용
};

vector <Node> node[100005];

char Tree[100005]; // Tree[] 배열에는 T,F,U 의 정보가 저장되어 있다. [ T -> TREE , F -> Fringe, U -> Unseen ]
int Fringe[100005]; // Fringe[] 배열에는 각 노드의 최소 거리가 저장되어 있다.
int path[100005]; // 최소 거리의 노드들의 path를 저장한 곳 ( 마지막에 각 지나온 최소 노드를 출력하기 위한 임시 배열 저장소 )

int main() {
	cin >> N >> M >> T; // 노드의 갯수, 간선의 갯수, 최소 거리 비용의 테스트 케이스 수를 차례로 입력 받음

	int x, y, z; // 조형물 1 번호 , 조형물 2 번호, 산책로 디자인 비용

	for (int i = 1; i <= M; i++) { // 간선의 수 만큼 반복하여 입력 받음
		cin >> x >> y >> z; // 조형물 1 번호 , 조형물 2 번호, 산책로 디자인 비용을 차례로 입력 받음
		node[x].push_back({ y,z }); // x(조형물 1)에 y(조형물 2)를 이어주고 비용을 추가
		node[y].push_back({ x,z }); // y(조형물 2)에 x(조형물 1)를 이어주고 비용을 추가
	}

	char p; // 질의 형식 체크
	int count = 1;

	for (int i = 1; i <= T; i++) { // 최소 거리 비용의 테스트 케이스 수만큼	반복
		cin >> p;

		if (p == 'P') { // 입력받은 질의형식이 P 일경우
			cin >> x; // 탐색을 시작할 초기 Vertex를 입력 받음

			/* Tree[]를 Unseen으로 초기화 하고,
				Fringe[]를 무한대 값으로 초기화 */
			for (int j = 0; j < 100005; j++) {
				Tree[j] = 'U';
				Fringe[j] = INT_MAX;
			}

			int a = x; // 마지막에 초기 vertex 값을 출력하기 위해 임시 변수 a에 저장함 (사실 안써도 되는데 로직을 순서를 잘못짬 )

			for (int k = 1; k < N; k++) { // 노드의 수만큼 반복하여, T와 F를 설정하고 각 최소 비용을 갱신함
				Tree[a] = 'T'; // 초기 입력된 값을 TREE로 만듬

				for (int n = 0; n < node[a].size(); n++) { // 현재 Tree로 설정된 vertex에 이어진 모든 노드만큼 반복하여 F로 설정 (만약, T라면 수행하지 않음)

					if (Tree[node[a][n].child] != 'T') { // T가 아니라면
						Tree[node[a][n].child] = 'F'; // F로 설정
					}

					/* CASE 1.
						Fringe의 값이 무한대 이면서 T가 아닐경우 -> 디자인의 비용을 바로 삽입 (아무것도 설정되지 않은 초기의 값이기 때문)*/
					if (Fringe[node[a][n].child] == INT_MAX && Tree[node[a][n].child] != 'T') {
						Fringe[node[a][n].child] = node[a][n].price; // 
					}
					/* CASE 2.
						Fringe의 값이 무한대가 아니면서 T가 아닐경우 -> 현재 저장된 디자인 비용과 현재 삽입할 디자인 비용중 가장 작은값으로 삽입 */
					else if (Fringe[node[a][n].child] != INT_MAX && Tree[node[a][n].child] != 'T') {
						if (Fringe[node[a][n].child] > node[a][n].price) {
							Fringe[node[a][n].child] = node[a][n].price;
						}
					}
				}

				/* 위에서 CASE 1 또는 CASE2를 거쳐 계산된 Fringe[] 배열에서 F인 값중에서 가장 작은 PATH를 찾기 */
				int min_ = INT_MAX;
				int number; // 가장 작은 PATH의 노드가 저장될 변수

				for (int n = 1; n <= N; n++) { // TREE에 모든 배열을 탐색함 ( 왜? -> F를 찾아서 대소비교를 해야되기 때문에 )
					if (Tree[n] == 'F') { // 하나씩 돌면서 배열에서 값이 F라면
						if (min_ > Fringe[n]) { // 대소 비교후 값이 작다면
							min_ = Fringe[n]; // min_에 가장 작은 값을 업데이트 해주고
							number = n; // number에는 찾은 배열에 노드의 번호의 수를 저장해 준다.
						}
						else if (min_ == Fringe[n]) { // 만약 값이 최소 비용의 값이 둘이 같다면
							number = min(number, n); // 두개의 노드중에 가장 작은 노드의 번호를 선택한다.
						}
					}
				}

				a = number; // 위에서 계산된 가장 작은 PATH의 노드를 다음 TREE로 만들기 위하여 a에 저장한다 -> 다시 TREE인 a부터 시작해서 반복적으로 수행함 
				path[count++] = a; // 마지막의 최소 비용의 지나온 노드의 PATH를 출력하기 위해 각 노드의 값을 임시적으로 저장함
			}
		}

		/* 위에서 모든 계산이 끝난 후 이제는 출력하는 부분 */
		int total = 0; // 모든 비용을 계산하기 위한 변수

		for (int b = 0; b < 100005; b++) {
			if (Fringe[b] != INT_MAX)
				total += Fringe[b]; // Fringe[] 배열을 전부 돌면서 INT_MAX(초기에 서치할때의 처음값) 이외의 값을 전부 더함
		}
		cout << total << ' '; // 총 합을 출력

		cout << x << ' '; // 초기 서치할때의 노드의 번호 출력
		for (int b = 1; b < 100005; b++) {
			if (path[b] != NULL)
				cout << path[b] << ' '; // 그 후 위에서 저장한 최소 비용의 지나온 노드의 PATH를 출력
		}cout << endl;

		/* 저장한 최소 비용의 지나온 노드의 PATH 배열을 초기화 하는 부분*/
		for (int b = 1; b < 100005; b++) {
			if (path[b] != NULL)
				path[b] = NULL;
		}
	}
}