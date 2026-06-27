#include "dashboard.hpp"

#include <imgui.h>

namespace mercury::ui
{

Dashboard::Dashboard(
    mercury::exchange::Gateway& gateway)
    :
    gateway_{gateway}
{
}

void Dashboard::render()
{
    ImGuiViewport* viewport =
        ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(
        viewport->WorkPos);

    ImGui::SetNextWindowSize(
        viewport->WorkSize);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse;

    ImGui::Begin(
        "Mercury",
        nullptr,
        flags);

    ImGui::TextColored(
        ImVec4(0.20f, 0.70f, 1.00f, 1.00f),
        "MERCURY EXCHANGE");

    ImGui::SameLine(
        ImGui::GetWindowWidth() - 120);

    ImGui::TextDisabled("SIMULATION");

    ImGui::Separator();

    ImGui::Columns(2, nullptr, false);

    render_order_entry();

    ImGui::Spacing();

    render_statistics();

    ImGui::NextColumn();

    render_order_book();

    ImGui::Spacing();

    render_recent_trades();

    ImGui::Columns(1);

    ImGui::End();
}

void Dashboard::render_order_entry()
{
    ImGui::SeparatorText("ORDER ENTRY");

    static int price = 10000;
    static int quantity = 100;

    ImGui::SetNextItemWidth(180);
    ImGui::InputInt(
        "Price",
        &price);

    ImGui::SetNextItemWidth(180);
    ImGui::InputInt(
        "Quantity",
        &quantity);

    ImGui::Spacing();

    ImGui::PushStyleColor(
        ImGuiCol_Button,
        ImVec4(0.10f, 0.60f, 0.20f, 1.00f));

    ImGui::Button(
        "BUY",
        ImVec2(120, 40));

    ImGui::PopStyleColor();

    ImGui::SameLine();

    ImGui::PushStyleColor(
        ImGuiCol_Button,
        ImVec4(0.75f, 0.20f, 0.20f, 1.00f));

    ImGui::Button(
        "SELL",
        ImVec2(120, 40));

    ImGui::PopStyleColor();
}
void Dashboard::render_order_book()
{
    ImGui::SeparatorText("ORDER BOOK");

    const auto snapshot =
        gateway_.snapshot();

    if (ImGui::BeginTable(
            "book",
            2,
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("Price");
        ImGui::TableSetupColumn("Volume");

        ImGui::TableHeadersRow();

        for (int i = 0; i < 5; ++i)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%llu",
                snapshot.asks[i].price);

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%u",
                snapshot.asks[i].volume);
        }

        for (int i = 0; i < 5; ++i)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%llu",
                snapshot.bids[i].price);

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%u",
                snapshot.bids[i].volume);
        }

        ImGui::EndTable();
    }
}

void Dashboard::render_recent_trades()
{
    ImGui::SeparatorText("RECENT TRADES");

    ImGui::TextDisabled(
        "No trades executed.");

    ImGui::Spacing();

    ImGui::TextDisabled(
        "Submit crossing");

    ImGui::TextDisabled(
        "orders to populate.");
}


void Dashboard::render_statistics()
{
    ImGui::SeparatorText("STATISTICS");

    ImGui::Text("Orders Processed");
    ImGui::TextColored(
        ImVec4(0.20f, 0.70f, 1.00f, 1.00f),
        "0");

    ImGui::Spacing();

    ImGui::Text("Trades");
    ImGui::TextColored(
        ImVec4(0.20f, 0.70f, 1.00f, 1.00f),
        "0");

    ImGui::Spacing();

    ImGui::Text("Latency");
    ImGui::TextColored(
        ImVec4(0.20f, 0.70f, 1.00f, 1.00f),
        "-- us");
}

} // namespace mercury::ui