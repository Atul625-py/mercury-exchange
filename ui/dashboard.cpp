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
    ImGui::Begin("Mercury Exchange");

    render_order_entry();

    ImGui::Separator();

    render_order_book();

    ImGui::Separator();

    render_recent_trades();

    ImGui::Separator();

    render_statistics();

    ImGui::End();
}

void Dashboard::render_order_entry()
{
    ImGui::Text("Order Entry");

    static int price = 10000;
    static int quantity = 100;

    ImGui::InputInt(
        "Price",
        &price);

    ImGui::InputInt(
        "Quantity",
        &quantity);

    ImGui::Button("Buy");

    ImGui::SameLine();

    ImGui::Button("Sell");
}

void Dashboard::render_order_book()
{
    ImGui::Text("Order Book");

    const auto snapshot =
        gateway_.snapshot();

    ImGui::Text(
        "Best Bid : %llu",
        snapshot.bids[0].price);

    ImGui::Text(
        "Best Ask : %llu",
        snapshot.asks[0].price);
}

void Dashboard::render_recent_trades()
{
    ImGui::Text("Recent Trades");

    ImGui::TextDisabled(
        "No trades yet.");
}

void Dashboard::render_statistics()
{
    ImGui::Text("Statistics");

    ImGui::BulletText(
        "Orders: --");

    ImGui::BulletText(
        "Trades: --");

    ImGui::BulletText(
        "Latency: --");
}

} // namespace mercury::ui