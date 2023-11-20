#include "RBord.hpp"
#include <iostream>
#include "CONST.hpp"
#include "Vector.hpp"
#include "Clik.hpp"


namespace COSTEMISE {

	static float dalta = 28.4f;
	static float border = 28.4f;
	static float coler1[4] = { 1.0f ,  1.0f ,  1.0f  ,  1.0f };
	static float coler2[4] = { 0.0f,  0.0f,  0.0f  ,  1.0f };
	
	static float Mcoler1[4] = { 0.0f ,  1.0f ,  1.0f  ,  1.0f };
	static float Mcoler2[4] = { 0.0f, 1.0f ,  1.0f  ,  1.0f };
	static float Mcoler[4] = { 1.0f ,  0.0f,  0.0f  ,  1.0f };
	static float backgroundColor[4] = { 0.0f, 0.0f, 0.0f ,  1.0f };
	static bool inBotom = false;
	Vector MOUSE;
}
 
 
void RBord::Set_Move(const std::list<Vector>& p_moves)
{
	this->MOVES.clear();
	this->MOVES = p_moves;
 
}

int RBord::Valid_move(Vector COORDONET)
{

	for (Vector pies : this->MOVES)
	{
		if (pies == COORDONET)
			return 1;
	}
	return 0;
}
 
void RBord::DrawChessboard(const ImVec2& startPos, const ImVec2& boardSize, const ImVec4& coler1, const ImVec4& coler2) {
	 
	using namespace COSTEMISE;
	ImVec2 squareSize(boardSize.x / 8, boardSize.y / 8);
 
	ImGuiIO& io = ImGui::GetIO();
	 
	int count = IM_ARRAYSIZE(io.MouseDown);

	// * (➡ , ⬇)
	for (int x = 0; x < 8; ++ x) {
		for (int y = 0; y < 8; ++ y) {

			int _x = abs((7 * inBotom) - x);
			int _y = abs((7 * inBotom) - y);

			ImVec2 squarePos = ImVec2(startPos.x + x * squareSize.x, startPos.y + y * squareSize.y);
			
			bool isWhite = (x + y) % 2 == 0;
			bool Mose = ((squarePos.x < io.MousePos.x) && (io.MousePos.x < squarePos.x + squareSize.x)) &&
				((squarePos.y < io.MousePos.y) && (io.MousePos.y < squarePos.y + squareSize.y));
			ImVec4 color;
			if (Mose)
			{
				MOUSE = {x , y};
				color = isWhite ? Mcoler1 : Mcoler2;
			}
			else
				if (this->Valid_move({ _x , _y }))
					color = Mcoler;
				else
					color = isWhite ? coler1 : coler2;
			ImGui::GetWindowDrawList()->AddRectFilled(squarePos, ImVec2(squarePos.x + squareSize.x, squarePos.y + squareSize.y), ImGui::GetColorU32(color));
			this->R_image(squarePos + ImVec2(border - startPos.x, border + dalta -startPos.y) ,
				         { squareSize.x  , squareSize.y  }, Vector(_x, _y));
		 
		}
	 
	}

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		Vector MOUSE((int)floor((io.MousePos.x - startPos.x) / squareSize.x),
			         (int)floor((io.MousePos.y - startPos.y) / squareSize.y));
		MOUSE.x = abs((7 * inBotom) - MOUSE.x);
		MOUSE.y = abs((7 * inBotom) - MOUSE.y);
		if (!((*this->BORD)[MOUSE] == 91))
			Move_Handler(MOUSE, *this);
		return;
	}
}

void RBord::R_image(const ImVec2& POSITHION, const ImVec2& dim ,const Vector&Pies)
{
 
		ImGui::SetCursorPos(POSITHION); // Set the desired position
		RBord::GetImage((*this->BORD)[Pies], dim);
 }

void  RBord::GetImage(int pies, ImVec2 dim)
{
	// todo  : Tranfer to map 
	switch (pies)
	{
		case 5:		ImGui::Image(this->wr->GetDescriptorSet(), dim);	break;
		case -5:	ImGui::Image(this->br->GetDescriptorSet(), dim);	break;
		case 2:		ImGui::Image(this->wn->GetDescriptorSet(), dim);	break;
		case    -2:	ImGui::Image(this->bn->GetDescriptorSet(), dim);	break;
		case     3:	ImGui::Image(this->wb->GetDescriptorSet(), dim);	break;
		case    -3:	ImGui::Image(this->bb->GetDescriptorSet(), dim);	break;
		case    15:	ImGui::Image(this->wq->GetDescriptorSet(), dim);	break;
		case   -15:	ImGui::Image(this->bq->GetDescriptorSet(), dim);	break;
		case  2401:	ImGui::Image(this->wk->GetDescriptorSet(), dim);	break;
		case -2401:	ImGui::Image(this->bk->GetDescriptorSet(), dim);	break;
		case   	 1:	ImGui::Image(this->wp->GetDescriptorSet(), dim);   break;
		case    -1:	ImGui::Image(this->bp->GetDescriptorSet(), dim);	break;
	}
}

void RBord::Rendere() {

	using namespace COSTEMISE;
	


	ImGui::Begin("Chessboard");
	{
		ImU32 bgColor = IM_COL32((int)(backgroundColor[0] * 255), (int)(backgroundColor[1] * 255), (int)(backgroundColor[2] * 255), (int)(backgroundColor[3] * 255));
		float _ = (ImGui::GetWindowSize()[0] + ImGui::GetWindowSize()[1]) / 2;
		ImGui::SetWindowSize(ImVec2( _,_ ));
 
		ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos(),
												  ImGui::GetWindowPos() + ImGui::GetWindowSize(),
			bgColor);

		DrawChessboard(ImGui::GetWindowPos() + ImVec2(border, border+dalta),
					   ImGui::GetWindowSize() + ImVec2(-border*2, -(border * 2) -dalta),
					   coler1, coler2);
 
	}
	ImGui::End();
 
	 ImGui::ShowDemoWindow();

}








 

void COSTEM::Rendere() {
	using namespace COSTEMISE;

	ImGui::Begin("costem");
	{
		if (ImGui::ImageButton(this->flipe->GetDescriptorSet(), ImVec2(25, 25)))
			inBotom = !inBotom;
		if (ImGui::IsItemHovered())
			ImGui::Text("flipe");

		ImGui::ColorEdit4("Color1", (float*)&coler1, 1 << 26);
		ImGui::ColorEdit4("Color2", (float*)&coler2, 1 << 26);
		ImGui::ColorEdit4("MColor1", (float*)&Mcoler1, 1 << 26);
		ImGui::ColorEdit4("MColor2", (float*)&Mcoler2, 1 << 26);
		ImGui::ColorEdit4("Mcoler", (float*)&Mcoler, 1 << 26);
		ImGui::ColorEdit4("backgroundColor", (float*)&backgroundColor, 1 << 26);
		ImGui::DragFloat("border", &border, 0.1f, 0, 0, "%.1f", 1);
		
		

		if (ImGui::TreeNode("Mouse State"))
		{
			ImGuiIO& io = ImGui::GetIO();
			if (ImGui::IsMousePosValid())
				ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
			else
				ImGui::Text("Mouse pos: <INVALID>");
			ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);

			int count = IM_ARRAYSIZE(io.MouseDown);
			ImGui::Text("Mouse down:");         for (int i = 0; i < count; i++) if (ImGui::IsMouseDown(i)) { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
			ImGui::Text("Mouse clicked:");      for (int i = 0; i < count; i++) if (ImGui::IsMouseClicked(i)) { ImGui::SameLine(); ImGui::Text("b%d (%d)", i, ImGui::GetMouseClickedCount(i)); }
			ImGui::Text("Mouse released:");     for (int i = 0; i < count; i++) if (ImGui::IsMouseReleased(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
			ImGui::Text("Mouse wheel: %.1f", io.MouseWheel);
			ImGui::Text("Pen Pressure: %.1f", io.PenPressure); // Note: currently unused
			ImGui::TreePop();
		}

	}
	ImGui::End();
}