#include "ToolInterface.h"

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include <string>

void ToolInterface::Init()
{
    auto orangeBullet = std::make_shared<BulletData>();
    orangeBullet->mName = "Orange";
    orangeBullet->mColor = ORANGE;
    orangeBullet->mSpeed = 200;
    orangeBullet->mSize = 10;
    orangeBullet->mAngularVelocity = 0.5f;
    mAllBullets.push_back(orangeBullet);

    auto purpleBullet = std::make_shared<BulletData>();
    purpleBullet->mName = "Purple";
    purpleBullet->mColor = PURPLE;
    purpleBullet->mSpeed = 200;
    purpleBullet->mSize = 10;
    purpleBullet->mAngularVelocity = 0.5f;
    mAllBullets.push_back(purpleBullet);

    auto blueBullet = std::make_shared<BulletData>();
    blueBullet->mName = "Blue";
    blueBullet->mColor = BLUE;
    blueBullet->mSpeed = 100;
    blueBullet->mSize = 20;
    blueBullet->mAngularVelocity = 0.0f;
    mAllBullets.push_back(blueBullet);

    AttackPatternData arthurPattern2 {};
    arthurPattern2.bulletData = orangeBullet;
    arthurPattern2.timeBetweenBullet = 0.15f;
    arthurPattern2.bulletCount = 2;
    arthurPattern2.rotationSpeed = 200;
    arthurPattern2.bulletRotationOffset = 0.0f;
    mAllAttackPattern.push_back(arthurPattern2);

    AttackPatternData arthurPattern {};
    arthurPattern.bulletData = purpleBullet;
    arthurPattern.timeBetweenBullet = 0.15f;
    arthurPattern.bulletCount = 2;
    arthurPattern.rotationSpeed = 200;
    //(PI / 180.0f) to convert in degrees
    arthurPattern.bulletRotationOffset = 90;
    mAllAttackPattern.push_back(arthurPattern);

    AttackPatternData clairePattern {};
    clairePattern.bulletData = blueBullet;
    clairePattern.timeBetweenBullet = 1.5f;
    clairePattern.bulletCount = 8;
    clairePattern.rotationSpeed = 6;
    clairePattern.bulletRotationOffset = 0.0f;
    mAllAttackPattern.push_back(clairePattern);

    UpdateBulletSpawner();
}

void ToolInterface::Draw()
{
	rlImGuiBegin();

    mBulletNames.clear();
    for (auto& bullet : mAllBullets)
    {
        mBulletNames.push_back(bullet->mName.c_str());
    }

    //ImGui::ShowDemoWindow();

	BulletEditor();
	AttackPatternEditor();

	rlImGuiEnd();
}

void ToolInterface::BulletEditor()
{
    ImGui::Begin("BulletEditor", NULL, { ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize });
    ImGui::SeparatorText("Creating a bullet");

    //create new bullet
    if (ImGui::Button("New Bullet +"))
    {
        auto newBullet = std::make_shared<BulletData>();
        newBullet->mName = "New Elaine's Bullet";

        mAllBullets.push_back(newBullet);
        mCurrentBulletIndex = mAllBullets.size() - 1;
    }

    //show all bullet already created
    ImGui::Combo("Bullets", &mCurrentBulletIndex, mBulletNames.data(), mBulletNames.size());

    auto& bullet = mAllBullets[mCurrentBulletIndex];

    //choose Name
    char newBulletName[64] = "";
    strcpy_s(newBulletName, bullet->mName.c_str());
    if (ImGui::InputTextWithHint("Name", "Choose name of bullet", newBulletName, IM_ARRAYSIZE(newBulletName)))
    {
        bullet->mName = newBulletName;
    }

    if (ImGui::TreeNodeEx("Bullet Data", ImGuiTreeNodeFlags_DefaultOpen))
    {
        //choose color
        float color[4] = {
            bullet->mColor.r / 255.0f,
            bullet->mColor.g / 255.0f,
            bullet->mColor.b / 255.0f,
            bullet->mColor.a / 255.0f
        };
        if (ImGui::ColorEdit4("Color", color))
        {
            bullet->mColor.r = (unsigned char) (color[0] * 255);
            bullet->mColor.g = (unsigned char) (color[1] * 255);
            bullet->mColor.b = (unsigned char) (color[2] * 255);
            bullet->mColor.a = (unsigned char) (color[3] * 255);
        }

        //choose image

        //choose speed
        ImGui::SliderFloat("Speed", &bullet->mSpeed, 0.0f, 500.0f);

        //choose size
        ImGui::SliderFloat("Size", &bullet->mSize, 5.0f, 50.0f);

        //choose angular velocity
        ImGui::DragFloat("Angular Velocity", &bullet->mAngularVelocity, 0.005f, 0.0f, 1.0f);


        //delete bullet
        static bool isBulletUsed = false;
        static bool isLastBullet = false;

        if (ImGui::Button("Delete Bullet"))
        {
            isBulletUsed = false;
            isLastBullet = false;

            for (int i = 0; i < mAllAttackPattern.size(); i++)
            {
                if (mAllAttackPattern[i].bulletData == bullet)
                {
                    isBulletUsed = true;
                    break;
                }
            }

            if (!isBulletUsed)
            {
                if (mAllBullets.size() > 1)
                {
                    isLastBullet = false;

                    if (bullet == mAllBullets.back())
                    {
                        mCurrentBulletIndex = 0;
                        mAllBullets.pop_back();
                    }
                    else
                    {
                        mAllBullets.erase(mAllBullets.begin() + mCurrentBulletIndex);
                        UpdateBulletSpawner();
                    }
                }
                else
                {
                    isLastBullet = true;
                }
            }
        }
        if (isBulletUsed)
        {
            ImGui::SameLine();
            ImGui::Text("Stop using this bullet so you can delete it.");
        }
        else if (isLastBullet)
        {
            ImGui::SameLine();
            ImGui::Text("You need to keep at least one bullet.");
        }

        ImGui::TreePop();
    }

    ImGui::End();
}

void ToolInterface::AttackPatternEditor()
{
    ImGui::Begin("AttackPattern Editor", NULL, { ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize });
    ImGui::SeparatorText("Creating an attack pattern");

    //create new attack pattern
    if (ImGui::Button("New Attack Pattern +"))
    {
        AttackPatternData data {};
        data.bulletData = mAllBullets[mCurrentBulletIndex];

        mAllAttackPattern.push_back(data);
        UpdateBulletSpawner();
    }


    for (int i = 0; i < mAllAttackPattern.size(); i++)
    {
        AttackPatternData& attackPattern = mAllAttackPattern[i];
        attackPattern.currentAttackPatternIndex = i;

        if (ImGui::TreeNodeEx(TextFormat("Attack Pattern %d", i), ImGuiTreeNodeFlags_DefaultOpen))
        {
            int bulletIndex = 0;
            for (int j = 0; j < mAllBullets.size(); j++)
            {
                if (attackPattern.bulletData == mAllBullets[j])
                {
                    bulletIndex = j;
                    break;
                }
            }

            //show all bullet already created
            if (ImGui::Combo("Bullets", &bulletIndex, mBulletNames.data(), mBulletNames.size()))
            {
                attackPattern.bulletData = mAllBullets[bulletIndex];
                UpdateBulletSpawner();
            }

            //choose bullet count
            if (ImGui::InputInt("Bullet Count", &attackPattern.bulletCount))
            {
                UpdateBulletSpawner();
            }

            //choose time between bullet
            if (ImGui::DragFloat("Fire Rate", &attackPattern.timeBetweenBullet, 0.005f, 0.0f, 5.0f))
            {
                UpdateBulletSpawner();
            }

            //choose rotation speed
            if (ImGui::DragFloat("Rotation Speed", &attackPattern.rotationSpeed, 1.0f, -360.0f, 360.0f))
            {
                UpdateBulletSpawner();
            }

            //choose time between bullet
            if (ImGui::DragFloat("Rotation Offset", &attackPattern.bulletRotationOffset, 1.0f , 0.0f, 360.0f))
            {
                UpdateBulletSpawner();
            }

            //delete attack pattern
            if (ImGui::Button("Delete Attack Pattern"))
            {
                //mGameManager.ToEraseObject(attackPattern);
                mAllAttackPattern.erase(mAllAttackPattern.begin() + attackPattern.currentAttackPatternIndex);
                UpdateBulletSpawner();
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();
}

void ToolInterface::SetBulletSpawner(std::shared_ptr<BulletSpawner> bulletSpawner)
{
    mBulletSpawner = bulletSpawner;
}

void ToolInterface::UpdateBulletSpawner()
{
    printf("Update of bullet spawner \n");

    mBulletSpawner->ClearAttackPatterns();

    for (int pattern = 0; pattern < mAllAttackPattern.size(); pattern++)
    {
        mBulletSpawner->AddAttackPattern(mAllAttackPattern[pattern]);
    }
}
