#include <iostream>
#include <string>

#include "imgui.h"
#include "Application.h"
#include "GuiUtils.h"
#include "Data/Day.h"
#include "Data/Meal.h"
#include "Controllers/AppManager.h"
#include "Controllers/UserController.h"
#include "Controllers/AppBehaviourController.h"

namespace MyApp
{
    namespace
    {
        // We are passing our own identifier to TableSetupColumn() to facilitate identifying columns in the sorting code.
        // This identifier will be passed down into ImGuiTableSortSpec::ColumnUserID.
        // But it is possible to omit the user id parameter of TableSetupColumn() and just use the column index instead! (ImGuiTableSortSpec::ColumnIndex)
        // If you don't use sorting, you will generally never care about giving column an ID!
        enum MyItemColumnID
        {
            MyItemColumnID_ID,
            MyItemColumnID_Name,
            MyItemColumnID_Action,
            MyItemColumnID_Quantity,
            MyItemColumnID_Description
        };

        struct MyItem
        {
            int         ID;
            const char* Name;
            int         Quantity;

            // We have a problem which is affecting _only this demo_ and should not affect your code:
            // As we don't rely on std:: or other third-party library to compile dear imgui, we only have reliable access to qsort(),
            // however qsort doesn't allow passing user data to comparing function.
            // As a workaround, we are storing the sort specs in a static/global for the comparing function to access.
            // In your own use case you would probably pass the sort specs to your sorting/comparing functions directly and not use a global.
            // We could technically call ImGui::TableGetSortSpecs() in CompareWithSortSpecs(), but considering that this function is called
            // very often by the sorting algorithm it would be a little wasteful.
            static const ImGuiTableSortSpecs* s_current_sort_specs;

            // Compare function to be used by qsort()
            static int CompareWithSortSpecsNew(const void* lhs, const void* rhs)
            {
                const MyItem* a = (const MyItem*)lhs;
                const MyItem* b = (const MyItem*)rhs;
                for (int n = 0; n < s_current_sort_specs->SpecsCount; n++)
                {
                    // Here we identify columns using the ColumnUserID value that we ourselves passed to TableSetupColumn()
                    // We could also choose to identify columns based on their index (sort_spec->ColumnIndex), which is simpler!
                    const ImGuiTableColumnSortSpecs* sort_spec = &s_current_sort_specs->Specs[n];
                    int delta = 0;
                    switch (sort_spec->ColumnUserID)
                    {
                    case MyItemColumnID_ID:             delta = (a->ID - b->ID);                break;
                    case MyItemColumnID_Name:           delta = (strcmp(a->Name, b->Name));     break;
                    case MyItemColumnID_Quantity:       delta = (a->Quantity - b->Quantity);    break;
                    case MyItemColumnID_Description:    delta = (strcmp(a->Name, b->Name));     break;
                    default: IM_ASSERT(0); break;
                    }
                    if (delta > 0)
                        return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? +1 : -1;
                    if (delta < 0)
                        return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? -1 : +1;
                }

                // qsort() is instable so always return a way to differenciate items.
                // Your own compare function may want to avoid fallback on implicit sort specs e.g. a Name compare if it wasn't already part of the sort specs.
                return (a->ID - b->ID);
            }
        };
        const ImGuiTableSortSpecs* MyItem::s_current_sort_specs = NULL;
    }


    void Start()
    {
        AppManager::Init();

        //TODO: for testing, remove in final version.
        for (int i = 0; i < 100; i++)
        {
            AppManager::GetUserController()->AddDay();
        }
    }

    void RenderUI()
    {
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        
        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", nullptr, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        ImGui::ShowDemoWindow();

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Menu"))
            {
                if (ImGui::MenuItem("Open File"))
                {
                    GuiUtils::OpenFile();
                }

                if (ImGui::MenuItem("Help window"))
                {
                    AppManager::GetAppBehController()->SetRenderHelpWindowBool(true);
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::Begin("Days statistics");

        if (ImGui::Button("Add New day"))
        {
            UserController* userController = dynamic_cast<UserController*>(AppManager::GetController<UserController>());
            userController->AddDay();
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Allow to add new day");

        if (ImGui::Button("All days Count"))
        {
            UserController* userController = dynamic_cast<UserController*>(AppManager::GetController<UserController>());
            std::cout << "Total days: " << userController->GetTotalDays() << std::endl;
        }

        //List with days-----------------------------------------------------------------------------
        //TODO: come up how to separate UI elements, maybe replace them into methods.
        static int track_item = 50;
        static bool enable_track = false;
        static bool enable_extra_decorations = true;
        static int is_day_selected = false;
        static std::string selected_day_string;

        ImGui::Checkbox("Decoration", &enable_extra_decorations);

        ImGui::Checkbox("Track", &enable_track);
        ImGui::PushItemWidth(100);
        ImGui::SameLine(140); enable_track |= ImGui::DragInt("##item", &track_item, 0.25f, 0, 99, "Item = %d");

        ImGui::BeginGroup();
        ImGui::TextUnformatted("Days:");

        //Days options--------------------------------------
        if (ImGui::Button("Options.."))
            ImGui::OpenPopup("Selected day option popup");
        ImGui::SameLine();
        ImGui::TextUnformatted(is_day_selected == false ? "<None>" : selected_day_string.c_str());
        if (ImGui::BeginPopup("Selected day option popup"))
        {
            std::string label = "";
            if (AppManager::GetUserController()->GetTotalDays() > 0)
            {
                label = AppManager::GetAppBehController()->GetSelectedDayIndex() == -1 ? "Select at least one day" : "Options..";
            }
            else
            {
                label = "Add at least one day to see options";
            }

            ImGui::Text(label.c_str());
            ImGui::Separator();

            if (AppManager::GetUserController()->GetTotalDays() > 0 && AppManager::GetAppBehController()->GetSelectedDayIndex() != -1)
            {
                ImGui::Selectable("Add new Meal", false);
                if (ImGui::IsItemClicked())
                {

                }

                ImGui::Selectable("Remove a last meal", false);
                if (ImGui::IsItemClicked())
                {

                }

                ImGui::Selectable("Delete day...", false);
                if (ImGui::IsItemClicked())
                {
                    ImGui::OpenPopup("Delete?");
                }
            }

            // Always center this window when appearing
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
                ImGui::Separator();

                //static int unused_i = 0;
                //ImGui::Combo("Combo", &unused_i, "Delete\0Delete harder\0");

                static bool dont_ask_me_next_time = false;
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
                ImGui::PopStyleVar();

                if (ImGui::Button("OK", ImVec2(120, 0)))
                {
                    AppManager::GetUserController()->RemoveDayByIndex(AppManager::GetAppBehController()->GetSelectedDayIndex());
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup(); //Delete? popup
            }

            ImGui::EndPopup(); //Selected day option popup
        }
        //--------------------------------------------------

        ImGui::EndGroup();

        if(ImGui::Button("Test button"))
        {
            UserController* testb = AppManager::GetController<UserController>();
            testb->GetAllDays()[0].GetConsumedProteins();
        }

        if (AppManager::GetAppBehController()->ShouldRenderHelpWindow())
        {
            RenderHelpWindow();
        }

        //----------------------------------------------------------

        static ImGuiTableFlags flags =
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
            | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
            | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
            | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
            | ImGuiTableFlags_SizingFixedFit;

        enum ContentsType { CT_Text, CT_Button, CT_SmallButton, CT_FillButton, CT_Selectable, CT_SelectableSpanRow };
        static int contents_type = CT_SelectableSpanRow;
        static int freeze_cols = 1;
        static int freeze_rows = 1;
        static ImVec2 outer_size_value = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing()* 12);
        static float row_min_height = 0.0f; // Auto
        static float inner_width_with_scroll = 0.0f; // Auto-extend
        static bool outer_size_enabled = true;
        static bool show_headers = true;


        static ImVector<int> selection;
        static bool items_need_sort = false;

         // Submit table
        const float inner_width_to_use = (flags & ImGuiTableFlags_ScrollX) ? inner_width_with_scroll : 0.0f;
        if (ImGui::BeginTable("table_advanced", 6, flags, outer_size_enabled ? outer_size_value : ImVec2(0, 0), inner_width_to_use))
        {
            // Declare columns
            // We use the "user_id" parameter of TableSetupColumn() to specify a user id that will be stored in the sort specifications.
            // This is so our sort function can identify a column given our own identifier. We could also identify them based on their index!
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHide, 0.0f, MyItemColumnID_ID);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Name);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, MyItemColumnID_Action);
            ImGui::TableSetupColumn("Meals", ImGuiTableColumnFlags_PreferSortDescending, 0.0f, MyItemColumnID_Quantity);
            ImGui::TableSetupColumn("Description", (flags & ImGuiTableFlags_NoHostExtendX) ? 0 : ImGuiTableColumnFlags_WidthStretch, 0.0f, MyItemColumnID_Description);
            ImGui::TableSetupColumn("Hidden", ImGuiTableColumnFlags_DefaultHide | ImGuiTableColumnFlags_NoSort);
            ImGui::TableSetupScrollFreeze(freeze_cols, freeze_rows);

            // Sort our data if sort specs have been changed! todo:: create sort for days
            //ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs();
            //if (sorts_specs && sorts_specs->SpecsDirty)
            //    items_need_sort = true;
            //if (sorts_specs && items_need_sort && items.Size > 1)
            //{
            //    MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
            //    qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecsNew);
            //    MyItem::s_current_sort_specs = NULL;
            //    sorts_specs->SpecsDirty = false;
            //}
            //items_need_sort = false;

            // Take note of whether we are currently sorting based on the Quantity field,
            // we will use this to trigger sorting when we know the data of this column has been modified.
            const bool sorts_specs_using_quantity = (ImGui::TableGetColumnFlags(3) & ImGuiTableColumnFlags_IsSorted) != 0;

            // Show headers
            if (show_headers)
                ImGui::TableHeadersRow();

            // Show data
            // FIXME-TABLE FIXME-NAV: How we can get decent up/down even though we have the buttons here?
            ImGui::PushButtonRepeat(true);
            // Demonstrate using clipper for large vertical lists
            ImGuiListClipper clipper;
            clipper.Begin(AppManager::GetUserController()->GetTotalDays());
            while (clipper.Step())
            {
                for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                {
                    Day currentDay = AppManager::GetUserController()->GetAllDays()[row_n];

                    const bool item_is_selected = selection.contains(row_n);
                    ImGui::PushID(row_n);
                    ImGui::TableNextRow(ImGuiTableRowFlags_None, row_min_height);

                    // For the demo purpose we can select among different type of items submitted in the first column
                    ImGui::TableSetColumnIndex(0);
                    char label[32];
                    sprintf_s(label, "%04d", row_n);
                    if (contents_type == CT_Text)
                        ImGui::TextUnformatted(label);
                    else if (contents_type == CT_Button)
                        ImGui::Button(label);
                    else if (contents_type == CT_SmallButton)
                        ImGui::SmallButton(label);
                    else if (contents_type == CT_FillButton)
                        ImGui::Button(label, ImVec2(-FLT_MIN, 0.0f));
                    else if (contents_type == CT_Selectable || contents_type == CT_SelectableSpanRow)
                    {
                        ImGuiSelectableFlags selectable_flags = (contents_type == CT_SelectableSpanRow) ? ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap : ImGuiSelectableFlags_None;
                        if (ImGui::Selectable(label, item_is_selected, selectable_flags, ImVec2(0, row_min_height)))
                        {
                            if (ImGui::GetIO().KeyCtrl)
                            {
                                //TODO: create multiple selection???
                                if (item_is_selected)
                                    selection.find_erase_unsorted(row_n);
                                else
                                    selection.push_back(row_n);
                            }
                            else
                            {
                                selection.clear();
                                selection.push_back(row_n);

                                is_day_selected = true;
                                selected_day_string = "Selected: ";
                                selected_day_string += label;
                                selected_day_string += " info about calories";
                                AppManager::GetAppBehController()->SetSelectedDayIndex(row_n);
                                std::cout << label << std::endl;
                            }
                        }
                    }

                    if (ImGui::TableSetColumnIndex(1))
                    {
                        char nameLabel[32];
                        sprintf_s(nameLabel, "day: %d", row_n);
                        ImGui::TextUnformatted(nameLabel);
                    }

                    // Here we demonstrate marking our data set as needing to be sorted again if we modified a quantity,
                    // and we are currently sorting on the column showing the Quantity.
                    // To avoid triggering a sort while holding the button, we only trigger it when the button has been released.
                    // You will probably need a more advanced system in your code if you want to automatically sort when a specific entry changes.
                    if (ImGui::TableSetColumnIndex(2))
                    {
                        if (ImGui::SmallButton("Add meal"))
                        {
                            std::cout << "Add meal to day pressed " << row_n << std::endl;

                            AppManager::GetAppBehController()->SetShouldRenderAddNewMealWindow(true);
                            //TODO: We break from for loop, but we have to break also from while

                            //TODO: to separate method??
                            is_day_selected = true;
                            selected_day_string = "Selected: ";
                            selected_day_string += label;
                            selected_day_string += " info about calories";
                            AppManager::GetAppBehController()->SetSelectedDayIndex(row_n);
                            std::cout << label << std::endl;
                        }

                        if (sorts_specs_using_quantity && ImGui::IsItemDeactivated()) { items_need_sort = true; }
                        ImGui::SameLine();

                        if (ImGui::SmallButton("Remove last meal"))
                        {
                            std::cout << "Last meal removed " << row_n << std::endl;
                        }

                        if (sorts_specs_using_quantity && ImGui::IsItemDeactivated()) { items_need_sort = true; }
                    }
                    
                    //TODO: something can be there.
                    if (ImGui::TableSetColumnIndex(3))
                    {
                        ImGui::Text("%d", AppManager::GetUserController()->GetDay(row_n)->GetMealCout());
                    }

                    if (ImGui::TableSetColumnIndex(4))
                    {
                        char description[124];
                        sprintf_s(description, "consumed calories: %d total Proteins: %d total Carbohydrates: %d Total fats: %d",
                            currentDay.GetTotalCalories(), currentDay.GetConsumedProteins(), currentDay.GetConsumedCarbohydrates(), currentDay.GetConsumedFats());
                        ImGui::Text(description);
                    }

                    if (ImGui::TableSetColumnIndex(5))
                        ImGui::Text("1234");

                    ImGui::PopID();
                }
            }
                ImGui::PopButtonRepeat();

                ImGui::EndTable();
        }

        //----------------------------------------------------------
        if (AppManager::GetAppBehController()->GetShouldRenderAddNewMealWindow())
        {
            ImGui::OpenPopup("Add Meal");
            RenderAddNewMealWindow();
        }

        ImGui::End(); //Days statistics

        ImGui::End(); //DockSpace
    }

    void RenderHelpWindow()
    {
        ImGui::Begin("Help window", nullptr, (ImGuiWindowFlags_NoCollapse));

        ImGui::Text("some text, user guide i'm alex, what to write idk just test \nsome new text on new line\none more line");
        ImGui::NewLine();

        ImGui::SameLine(150);
        if (ImGui::Button("Close", ImVec2(100, 0)))
        {
            AppManager::GetAppBehController()->SetRenderHelpWindowBool(false);
        }

        ImGui::End();
    }

    void RenderAddNewMealWindow()
    {
        //Implement add meal logic.

        if (ImGui::BeginPopupModal("Add Meal", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Add consumed information.\n");
            ImGui::Separator();

            static int proteins = 0;
            static int carbohydrates = 0;
            static int fats = 0;
            ImGui::InputInt("Input Proteins", &proteins);
            ImGui::InputInt("Input Carbohydrates", &carbohydrates);
            ImGui::InputInt("Input Fats", &fats);

            if (ImGui::Button("Add", ImVec2(120, 0)))
            {
                AppManager::GetAppBehController()->SetShouldRenderAddNewMealWindow(false);

                Day* day = AppManager::GetUserController()->GetDay(AppManager::GetAppBehController()->GetSelectedDayIndex());
                if (day != nullptr)
                {
                    day->AddMeal(proteins, carbohydrates, fats);
                }

                proteins = 0;
                carbohydrates = 0;
                fats = 0;

                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                AppManager::GetAppBehController()->SetShouldRenderAddNewMealWindow(false);

                proteins = 0;
                carbohydrates = 0;
                fats = 0;

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup(); //Delete? popup
        }
    }
}
