#include "HexGrid.h"
#include "AI.h"
#include <windows.h>
#include <string>
#include <sstream>
#include <cmath>

#define M_PI 3.14159265358979323846

// Window dimensions
const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 700;

// Hex rendering
const double HEX_SIZE = 25.0;
const double HEX_OFFSET_X = 100.0;
const double HEX_OFFSET_Y = 100.0;

// Game state
HexGrid* g_grid = nullptr;
AI* g_ai = nullptr;
bool g_aiEnabled = true;
bool g_gameOver = false;
int g_moveCount = 0;
HexCoord g_hoveredHex(-1, -1);
MoveInfo g_lastAIMove = {};
bool g_aiThinking = false;

// Forward declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void DrawHexBoard(HDC hdc);
void DrawHex(HDC hdc, const HexCoord& coord, bool hovered);
void DrawStone(HDC hdc, int x, int y, Player player);
void DrawInfoPanel(HDC hdc);
POINT HexToPixel(const HexCoord& coord);
HexCoord PixelToHex(int x, int y);
void ProcessMove(HWND hwnd, const HexCoord& coord);
void MakeAIMove(HWND hwnd);
void NewGame(HWND hwnd);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize game
    g_grid = new HexGrid();
    g_ai = new AI();  // Single difficulty - always plays optimally
    
    // Register window class
    const char CLASS_NAME[] = "HexGameClass";
    
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    
    RegisterClassA(&wc);
    
    // Create window
    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "Hex Game - AI Project",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    
    if (hwnd == NULL) {
        return 0;
    }
    
    ShowWindow(hwnd, nCmdShow);
    
    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Cleanup
    delete g_grid;
    delete g_ai;
    
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Double buffering
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
            SelectObject(memDC, memBitmap);
            
            // Clear background
            RECT rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
            HBRUSH bgBrush = CreateSolidBrush(RGB(245, 245, 245));
            FillRect(memDC, &rect, bgBrush);
            DeleteObject(bgBrush);
            
            // Draw everything
            DrawHexBoard(memDC);
            DrawInfoPanel(memDC);
            
            // Copy to screen
            BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, memDC, 0, 0, SRCCOPY);
            
            // Cleanup
            DeleteObject(memBitmap);
            DeleteDC(memDC);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            HexCoord newHovered = PixelToHex(x, y);
            
            if (!(newHovered == g_hoveredHex)) {
                g_hoveredHex = newHovered;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }
        
        case WM_LBUTTONDOWN: {
            if (g_gameOver || g_aiThinking || g_grid->getCurrentPlayer() != Player::RED) {
                return 0;
            }
            
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            HexCoord coord = PixelToHex(x, y);
            
            if (coord.q >= 0 && coord.q < HexGrid::BOARD_SIZE &&
                coord.r >= 0 && coord.r < HexGrid::BOARD_SIZE) {
                ProcessMove(hwnd, coord);
            }
            return 0;
        }
        
        case WM_KEYDOWN: {
            if (wParam == 'N') {  // N key for New Game
                NewGame(hwnd);
            } else if (wParam == 'Z' && (GetKeyState(VK_CONTROL) & 0x8000)) {  // Ctrl+Z for Undo
                if (!g_grid->getMoveHistory().empty()) {
                    g_grid->undoMove();
                    if (g_aiEnabled && !g_grid->getMoveHistory().empty()) {
                        g_grid->undoMove();
                    }
                    g_moveCount = g_grid->getMoveHistory().size();
                    g_gameOver = false;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            return 0;
        }
        
        case WM_USER + 1: {  // Custom message for AI move
            // This case is no longer used - AI now runs directly in ProcessMove
            return 0;
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

POINT HexToPixel(const HexCoord& coord) {
    double x = HEX_SIZE * (sqrt(3.0) * coord.q + sqrt(3.0) / 2.0 * coord.r);
    double y = HEX_SIZE * (3.0 / 2.0 * coord.r);
    
    return {(LONG)(x + HEX_OFFSET_X), (LONG)(y + HEX_OFFSET_Y)};
}

HexCoord PixelToHex(int px, int py) {
    double x = (px - HEX_OFFSET_X) / HEX_SIZE;
    double y = (py - HEX_OFFSET_Y) / HEX_SIZE;
    
    double q = (sqrt(3.0) / 3.0 * x - 1.0 / 3.0 * y);
    double r = (2.0 / 3.0 * y);
    double s = -q - r;
    
    int rq = (int)round(q);
    int rr = (int)round(r);
    int rs = (int)round(s);
    
    double q_diff = abs(rq - q);
    double r_diff = abs(rr - r);
    double s_diff = abs(rs - s);
    
    if (q_diff > r_diff && q_diff > s_diff) {
        rq = -rr - rs;
    } else if (r_diff > s_diff) {
        rr = -rq - rs;
    }
    
    HexCoord result(rq, rr);
    
    if (result.q >= 0 && result.q < HexGrid::BOARD_SIZE &&
        result.r >= 0 && result.r < HexGrid::BOARD_SIZE) {
        return result;
    }
    
    return HexCoord(-1, -1);
}

void DrawHexBoard(HDC hdc) {
    for (const auto& kv : g_grid->getGrid()) {
        DrawHex(hdc, kv.first, kv.first == g_hoveredHex);
    }
    
    // Draw edge highlights - RED (top and bottom)
    HPEN redPen = CreatePen(PS_SOLID, 3, RGB(231, 76, 60));
    HBRUSH redBrush = (HBRUSH)GetStockObject(NULL_BRUSH);  // Hollow brush
    HPEN oldPen = (HPEN)SelectObject(hdc, redPen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, redBrush);
    
    for (const HexCoord& coord : g_grid->getTopEdge()) {
        POINT pt = HexToPixel(coord);
        Ellipse(hdc, pt.x - 30, pt.y - 30, pt.x + 30, pt.y + 30);
    }
    for (const HexCoord& coord : g_grid->getBottomEdge()) {
        POINT pt = HexToPixel(coord);
        Ellipse(hdc, pt.x - 30, pt.y - 30, pt.x + 30, pt.y + 30);
    }
    
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(redPen);
    
    // Draw edge highlights - BLUE (left and right)
    HPEN bluePen = CreatePen(PS_SOLID, 3, RGB(52, 152, 219));
    HBRUSH blueBrush = (HBRUSH)GetStockObject(NULL_BRUSH);  // Hollow brush
    oldPen = (HPEN)SelectObject(hdc, bluePen);
    oldBrush = (HBRUSH)SelectObject(hdc, blueBrush);
    
    for (const HexCoord& coord : g_grid->getLeftEdge()) {
        POINT pt = HexToPixel(coord);
        Ellipse(hdc, pt.x - 30, pt.y - 30, pt.x + 30, pt.y + 30);
    }
    for (const HexCoord& coord : g_grid->getRightEdge()) {
        POINT pt = HexToPixel(coord);
        Ellipse(hdc, pt.x - 30, pt.y - 30, pt.x + 30, pt.y + 30);
    }
    
    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);
    DeleteObject(bluePen);
}

void DrawHex(HDC hdc, const HexCoord& coord, bool hovered) {
    POINT center = HexToPixel(coord);
    POINT points[6];
    
    for (int i = 0; i < 6; ++i) {
        double angle = M_PI / 3.0 * i;
        points[i].x = center.x + (LONG)(HEX_SIZE * cos(angle));
        points[i].y = center.y + (LONG)(HEX_SIZE * sin(angle));
    }
    
    // Draw hex cell
    HPEN hexPen = CreatePen(PS_SOLID, hovered ? 3 : 1, hovered ? RGB(255, 215, 0) : RGB(204, 204, 204));
    HBRUSH hexBrush = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, hexPen);
    SelectObject(hdc, hexBrush);
    
    Polygon(hdc, points, 6);
    
    DeleteObject(hexPen);
    DeleteObject(hexBrush);
    
    // Draw stone
    Player player = g_grid->getCell(coord);
    if (player != Player::NONE) {
        DrawStone(hdc, center.x, center.y, player);
    }
}

void DrawStone(HDC hdc, int x, int y, Player player) {
    COLORREF color = (player == Player::RED) ? RGB(231, 76, 60) : RGB(52, 152, 219);
    
    // Shadow
    HBRUSH shadowBrush = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, shadowBrush);
    SelectObject(hdc, GetStockObject(NULL_PEN));
    Ellipse(hdc, x - 16, y - 16, x + 20, y + 20);
    DeleteObject(shadowBrush);
    
    // Stone
    HBRUSH stoneBrush = CreateSolidBrush(color);
    HPEN stonePen = CreatePen(PS_SOLID, 2, color);
    SelectObject(hdc, stoneBrush);
    SelectObject(hdc, stonePen);
    Ellipse(hdc, x - 18, y - 18, x + 18, y + 18);
    
    DeleteObject(stoneBrush);
    DeleteObject(stonePen);
}

void DrawInfoPanel(HDC hdc) {
    int panelX = 750;
    int y = 20;
    
    SetBkMode(hdc, TRANSPARENT);
    
    // Title
    HFONT titleFont = CreateFontA(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                 CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                 DEFAULT_PITCH | FF_SWISS, "Arial");
    SelectObject(hdc, titleFont);
    SetTextColor(hdc, RGB(0, 0, 0));
    TextOutA(hdc, panelX, y, "Hex Game", 8);
    y += 50;
    
    DeleteObject(titleFont);
    
    // Info font
    HFONT infoFont = CreateFontA(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                DEFAULT_PITCH | FF_SWISS, "Arial");
    SelectObject(hdc, infoFont);
    
    // Current turn
    Player current = g_grid->getCurrentPlayer();
    SetTextColor(hdc, (current == Player::RED) ? RGB(231, 76, 60) : RGB(52, 152, 219));
    std::string turnText = (current == Player::RED) ? "Turn: RED" : "Turn: BLUE";
    TextOutA(hdc, panelX, y, turnText.c_str(), turnText.length());
    y += 40;
    
    // Game status
    if (g_gameOver) {
        Player winner = g_grid->getWinner();
        SetTextColor(hdc, (winner == Player::RED) ? RGB(231, 76, 60) : RGB(52, 152, 219));
        std::string winText = (winner == Player::RED) ? "RED WINS!" : "BLUE WINS!";
        TextOutA(hdc, panelX, y, winText.c_str(), winText.length());
        y += 40;
    }
    
    // Show thinking indicator
    if (g_aiThinking) {
        SetTextColor(hdc, RGB(52, 152, 219));
        TextOutA(hdc, panelX, y, "AI is thinking...", 17);
        y += 40;
    }
    
    y += 30;
    
    // Simple player goals
    SetTextColor(hdc, RGB(231, 76, 60));
    TextOutA(hdc, panelX, y, "RED: Top to Bottom", 18);
    y += 30;
    SetTextColor(hdc, RGB(52, 152, 219));
    TextOutA(hdc, panelX, y, "BLUE: Left to Right", 19);
    
    DeleteObject(infoFont);
}

void ProcessMove(HWND hwnd, const HexCoord& coord) {
    if (g_grid->getCell(coord) != Player::NONE) {
        return;
    }
    
    g_grid->makeMove(coord);
    g_moveCount++;
    InvalidateRect(hwnd, NULL, FALSE);
    
    // Force immediate paint to show player's move
    UpdateWindow(hwnd);
    
    // Process all pending messages to ensure screen updates
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) break;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Check for winner after player's move
    Player winner = g_grid->getWinner();
    if (winner != Player::NONE) {
        g_gameOver = true;
        InvalidateRect(hwnd, NULL, FALSE);
        UpdateWindow(hwnd);
        
        std::string msg = (winner == Player::RED) ? "RED WINS!" : "BLUE WINS!";
        MessageBoxA(hwnd, msg.c_str(), "Game Over", MB_OK | MB_ICONINFORMATION);
        return;
    }
    
    if (g_aiEnabled && g_grid->getCurrentPlayer() == Player::BLUE) {
        g_aiThinking = true;
        InvalidateRect(hwnd, NULL, FALSE);
        UpdateWindow(hwnd);
        
        // Process messages again to show "thinking" indicator
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        // Small delay so user sees their move
        Sleep(150);
        
        // Now calculate AI move
        MakeAIMove(hwnd);
    }
}

void MakeAIMove(HWND hwnd) {
    g_lastAIMove = g_ai->calculateMove(*g_grid);
    
    if (g_lastAIMove.move.coord.q >= 0 && g_lastAIMove.move.coord.q < HexGrid::BOARD_SIZE &&
        g_lastAIMove.move.coord.r >= 0 && g_lastAIMove.move.coord.r < HexGrid::BOARD_SIZE) {
        
        bool success = g_grid->makeMove(g_lastAIMove.move.coord);
        
        if (success) {
            g_moveCount++;
            g_aiThinking = false;
            InvalidateRect(hwnd, NULL, FALSE);
            UpdateWindow(hwnd);
            
            // Check for winner after AI's move
            Player winner = g_grid->getWinner();
            if (winner != Player::NONE) {
                g_gameOver = true;
                InvalidateRect(hwnd, NULL, FALSE);
                UpdateWindow(hwnd);
                
                std::string msg = (winner == Player::RED) ? "RED WINS!" : "BLUE WINS!";
                MessageBoxA(hwnd, msg.c_str(), "Game Over", MB_OK | MB_ICONINFORMATION);
            }
        } else {
            g_aiThinking = false;
            InvalidateRect(hwnd, NULL, FALSE);
        }
    } else {
        g_aiThinking = false;
        InvalidateRect(hwnd, NULL, FALSE);
    }
}

void NewGame(HWND hwnd) {
    g_grid->reset();
    g_gameOver = false;
    g_moveCount = 0;
    g_aiThinking = false;
    g_lastAIMove = MoveInfo{};
    InvalidateRect(hwnd, NULL, FALSE);
}
