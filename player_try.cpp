#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits.h>
#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)
#define DD 5

int setboard[8][8];
int player;
const int SIZE = 8;
std::array<std::array<int, 8>, 8> board;
void setvalue(void);

struct Point {
    int x, y;
    Point() : Point(0, 0) {}
	Point(float x, float y) : x(x), y(y) {}
    Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
    bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
};

Point Ahoy;
std::vector<Point> next_valid_spots;

class State{
private:
    Point directions[8];
    int get_disc(Point p) const {
        return next_board[p.x][p.y];
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < 8 && 0 <= p.y && p.y < 8;
    }
    void set_disc(Point p, int disc) {
        next_board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p)) return false;
        if (get_disc(p) != disc) return false;
        return true;
    }
    int get_next_player(int player) const { return 3 - player;}
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != 0) return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player))) continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != 0) {
                if (is_disc_at(p, cur_player)) return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player))) continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != 0) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) set_disc(s, cur_player);
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    std::array<std::array<int, 8>, 8> next_board;
    std::vector<State> next_valids;
    std::vector<Point> next_va;
    std::array<int, 3> disc_count;
    Point center;
    int value;
    int cur_player;
    State(Point p){
        Point center=p;
        value = setboard[p.x][p.y];
        //update new value
        for(int i=0;i<8;i++) for(int j=0;j<8;j++){
            if(next_board[i][j] == 0) disc_count[0]++;
            else if(next_board[i][j] == 1) disc_count[1]++;
            else if(next_board[i][j] == 2) disc_count[2]++;
        }

    }
    State(std::array<std::array<int, 8>,8>& b, std::vector<Point>& na, int pla){
        cur_player = pla;
        for(int i=0;i<8;i++) for(int j=0;j<8;j++) next_board[i][j] = b[i][j], disc_count[next_board[i][j]]++;
        next_va = na;
        set();
    }
    
    void set_dir(){
        directions[0].x=-1,directions[0].y=-1;directions[1].x=-1,directions[1].y=0;
        directions[2].x=-1,directions[2].y=1;directions[3].x=0,directions[3].y=-1;
        directions[4].x=0,directions[4].y=1;directions[5].x=1,directions[5].y=-1;
        directions[6].x=1,directions[6].y=0;directions[7].x=1,directions[7].y=1;
    }

    void set(){
        value = disc_count[1]-disc_count[2]+(corner(1)-corner(2))*100+(buffer(1)-buffer(2))*(-30)+(edge(1)-edge(2))*10;
        //buffer 跟 edge 一定要是三倍的關係 ---by my tseting
    }

    int corner(int type){
        int t=0;
        if(next_board[0][0]==type) t++;
        if(next_board[0][7]==type) t++;
        if(next_board[7][0]==type) t++;
        if(next_board[7][7]==type) t++;
        return t;
    }

    int buffer(int type){
        int t=0;
        if(next_board[0][1]==type) t++;
        if(next_board[1][1]==type) t++;
        if(next_board[1][0]==type) t++;
        if(next_board[0][6]==type) t++;
        if(next_board[1][6]==type) t++;
        if(next_board[1][7]==type) t++;
        if(next_board[6][0]==type) t++;
        if(next_board[6][1]==type) t++;
        if(next_board[7][1]==type) t++;
        if(next_board[6][6]==type) t++;
        if(next_board[7][6]==type) t++;
        if(next_board[6][7]==type) t++;
        return t;
    }

    int edge(int type){
        int t=0;
        if(next_board[0][2]==type) t++;
        if(next_board[0][3]==type) t++;
        if(next_board[0][4]==type) t++;
        if(next_board[0][5]==type) t++;
        if(next_board[2][0]==type) t++;
        if(next_board[3][0]==type) t++;
        if(next_board[4][0]==type) t++;
        if(next_board[5][0]==type) t++;
        if(next_board[2][7]==type) t++;
        if(next_board[3][7]==type) t++;
        if(next_board[4][7]==type) t++;
        if(next_board[5][7]==type) t++;
        if(next_board[7][2]==type) t++;
        if(next_board[7][3]==type) t++;
        if(next_board[7][4]==type) t++;
        if(next_board[7][5]==type) t++;
        return t;
    }

    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (next_board[i][j] != 0) continue;
                if (is_spot_valid(p)) valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }

    void update(Point next){
        set_dir();
        //put first
        set_disc(next,cur_player);
        disc_count[cur_player]++;
        disc_count[0]--;
        //flip
        flip_discs(next);
        //update its value
        set();
        //search a spot to put
        cur_player = get_next_player(cur_player);
        next_va = get_valid_spots();
    }
};



void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    float x, y;
    for (unsigned long i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

void setvalue(void){
    //corner capture
    /*
    99,  -8,   8,  6,  6,  8,  -8, 99 
	-8, -24,  -4, -3, -3, -4, -24, -8  
	 8,  -4,   7,  4,  4,  7,  -4,  8 
	 6,  -3,   4,  0,  0,  4,  -3,  6  
	 6,  -3,   4,  0,  0,  4,  -3,  6 
	 8,  -4,   7,  4,  4,  7,  -4,  8 
	-8, -24,  -4, -3, -3, -4, -24, -8 
	99,  -8,   8,  6,  6,  8,  -8, 99 
    */
    setboard[0][0]=99,setboard[0][1]=-8,setboard[0][2]=8,setboard[0][3]=6,
    setboard[0][4]=6,setboard[0][5]=8,setboard[0][6]=-8,setboard[0][7]=99;

    setboard[1][0]=-8,setboard[1][1]=-24,setboard[1][2]=-4,setboard[1][3]=-3,
    setboard[1][4]=-3,setboard[1][5]=-4,setboard[1][6]=-24,setboard[1][7]=-8;
    
    setboard[2][0]=8,setboard[2][1]=-4,setboard[2][2]=7,setboard[2][3]=4,
    setboard[2][4]=4,setboard[2][5]=7,setboard[2][6]=-4,setboard[2][7]=8;
    
    setboard[3][0]=6,setboard[3][1]=-3,setboard[3][2]=4,setboard[3][3]=0,
    setboard[3][4]=0,setboard[3][5]=4,setboard[3][6]=-3,setboard[3][7]=6;
    
    setboard[4][0]=6,setboard[4][1]=-3,setboard[4][2]=4,setboard[4][3]=0,
    setboard[4][4]=0,setboard[4][5]=4,setboard[4][6]=-3,setboard[4][7]=6;

    setboard[5][0]=8,setboard[5][1]=-4,setboard[5][2]=7,setboard[5][3]=4,
    setboard[5][4]=4,setboard[5][5]=7,setboard[5][6]=-4,setboard[5][7]=8;

    setboard[6][0]=-8,setboard[6][1]=-24,setboard[6][2]=-4,setboard[6][3]=-3,
    setboard[6][4]=-3,setboard[6][5]=-4,setboard[6][6]=-24,setboard[6][7]=-8;

    setboard[7][0]=99,setboard[7][1]=-8,setboard[7][2]=8,setboard[7][3]=6,
    setboard[7][4]=6,setboard[7][5]=8,setboard[7][6]=-8,setboard[7][7]=99;
}

int alpha_beta(State cur, int depth, int alpha, int beta ,bool player_mode){
    if(cur.next_va.size() == 0 || depth == 0) return cur.value;
    if(player_mode){
        int good=INT_MIN;
        for(Point it:cur.next_va){
            State nextstate=cur;
            nextstate.update(it);
            int rushia = alpha_beta(nextstate,depth-1,alpha,beta,false);
            if(good < rushia){
                good = rushia;
                if(depth == DD) Ahoy = it;
            }
            alpha = max(good,alpha);
            if(alpha>=beta) break;
        }
        return good;
    }else{
        int good=INT_MAX;
        for(Point it:cur.next_va){
            State nextstate=cur;
            nextstate.update(it);
            int rushia = alpha_beta(nextstate,depth-1,alpha,beta,true);
            if(good > rushia){
                good = rushia;
                if(depth == DD) Ahoy = it;
            }
            beta = min(good,beta);
            if(alpha>=beta) break;
        }
        return good;
    }
}

int Minimax(State cur, int depth, bool player_mode){
    if(cur.next_va.size() == 0 || depth == 0) return cur.value;
    if(player_mode){
        int good=INT_MIN;
        for(Point it:cur.next_va){
            State nextstate=cur;
            nextstate.update(it);
            int rushia = Minimax(nextstate,depth-1,false);
            if(good < rushia){
                good = rushia;
                if(depth == DD) Ahoy = it;
            }
        }
        return good;
    }else{
        int good=INT_MAX;
        for(Point it:cur.next_va){
            State nextstate=cur;
            nextstate.update(it);
            int rushia = Minimax(nextstate,depth-1,true);
            if(good > rushia){
                good = rushia;
                if(depth == DD) Ahoy = it;
            }
        }
        return good;
    }
}

int getvalue(){
    int max=-100,io=0,va;
    for(int i=0;i<next_valid_spots.size();i++){
        Point p=next_valid_spots[i];
        State n = State(p);
        va = alpha_beta(n,DD,INT_MIN,INT_MAX,true);
        if(max<va) max=va,io=i;
    }
    return io;
}

void write_valid_spot(std::ofstream& fout) {
    /*
    setvalue();
    int index = getvalue();
    Point p = next_valid_spots[index];*/
    State s( board, next_valid_spots, player );
    alpha_beta(s,DD,INT_MIN,INT_MAX,true);
    //Minimax(s,DD,true);
    // Remember to flush the output to ensure the last action is written to file.
    fout << Ahoy.x << " " << Ahoy.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
