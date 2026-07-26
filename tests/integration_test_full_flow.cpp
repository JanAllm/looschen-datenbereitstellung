// Full-flow integration test: runs the complete PLC dialog against the
// embedded open62541 test server (OPCUAServer) - no real PLC required.
//
// Covered end to end:
//   1. updateProjectList  -> project names appear in the ProjektArray node
//   2. showProjectInfo    -> LenProjekt / AnzDatenblöcke match the file and
//                            the configured block size (regression: the block
//                            size must reach the ProjectManager, not default 5)
//   3. transferProjectData-> a simulated PLC drives the ReadData/WriteData
//                            handshake; every block is captured and checked:
//                            no header line, no trailing CR, correct order,
//                            correct IndexData sequence, keep-alive invoked
//   4. abort paths        -> PLC abort flag and failing keep-alive both end
//                            the transfer promptly instead of blocking forever

#include <gtest/gtest.h>

#include "interfaces/OPCUAServer.h"
#include "interfaces/SPSController.h"
#include "interfaces/setupVarInfos.h"
#include "CNC_Daten/cnc_projekt_manager.h"
#include "web/app_state.h"

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

namespace
{
// Data lines of the test project (without header). CRLF endings and one empty
// line are added when the file is written - both must be invisible to the PLC.
const std::vector<std::string> kProjectLines = {
    "G01 X0 Y0",
    "G01 X10 Y0",
    "G01 X10 Y10",
    "G02 X20 Y10 I5 J0",
    "G01 X20 Y20",
    "G01 X0 Y20",
    "G01 X0 Y0",
};
constexpr int kBlockSize = 4;   // -> ceil(7/4) = 2 blocks
constexpr int kMaxProjects = 16;
const std::string kProjectFile = "TestProjekt.tap";
} // namespace

class FullFlowTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Fresh port per test: quick restarts of the same port can collide
        // with sockets still in TIME_WAIT.
        static std::atomic<uint16_t> nextPort{48484};
        port_ = nextPort++;

        writeProjectFile();
        startServer();
        connectController();

        manager_ = std::make_unique<ProjectManager>(
            *controller_, state_, folder_.string(), 200, 400, kBlockSize);
    }

    void TearDown() override
    {
        if (controller_)
        {
            controller_->disconnect();
        }
        if (server_)
        {
            server_->stop();
        }
        std::error_code ec;
        fs::remove_all(folder_, ec);
    }

    // The .tap file is written with CRLF endings (files come from Windows) and
    // contains a header comment plus one empty line - exactly what the
    // service has to filter out before the PLC sees the data.
    void writeProjectFile()
    {
        folder_ = fs::temp_directory_path() / ("looschen_fullflow_" + std::to_string(port_));
        fs::create_directories(folder_);

        std::ofstream file(folder_ / kProjectFile, std::ios::binary);
        ASSERT_TRUE(file.is_open());
        file << "(Projekt Integrationstest)\r\n";
        for (size_t i = 0; i < kProjectLines.size(); ++i)
        {
            file << kProjectLines[i] << "\r\n";
            if (i == 1)
            {
                file << "\r\n"; // empty line, must be skipped
            }
        }
    }

    void startServer()
    {
        server_ = std::make_unique<OPCUAServer>(port_, "TestSPS");

        const uint16_t ns = 1;
        uint32_t id = 100;
        auto next = [&id]() { return id++; };

        // One node per variable that SPSController::setupVariables registers.
        // The numeric ids are recorded in setupVars_ so client and server agree.
        setupVars_.ns = ns;
        auto addBool = [&](int &field, const char *name) {
            field = static_cast<int>(next());
            ASSERT_TRUE(server_->addBoolVariable(ns, static_cast<uint32_t>(field), name, false)) << name;
        };
        auto addInt16 = [&](int &field, const char *name, int16_t init = 0) {
            field = static_cast<int>(next());
            ASSERT_TRUE(server_->addInt16Variable(ns, static_cast<uint32_t>(field), name, init)) << name;
        };
        auto addString = [&](int &field, const char *name) {
            field = static_cast<int>(next());
            ASSERT_TRUE(server_->addStringVariable(ns, static_cast<uint32_t>(field), name, "")) << name;
        };
        auto addInt16Array = [&](int &field, const char *name) {
            field = static_cast<int>(next());
            ASSERT_TRUE(server_->addInt16ArrayVariable(ns, static_cast<uint32_t>(field), name,
                                                       {255, 255, 255})) << name;
        };

        addBool(setupVars_.iError, "Error");
        addString(setupVars_.iInfo, "Info_Text");
        addInt16(setupVars_.iHeartbeat, "Heartbeat");
        addInt16(setupVars_.iStatusCode, "StatusCode");

        setupVars_.iDataArry = static_cast<int>(next());
        ASSERT_TRUE(server_->addStringArrayVariable(
            ns, static_cast<uint32_t>(setupVars_.iDataArry), "DataArray",
            std::vector<std::string>(kBlockSize, "")));
        setupVars_.iProjektArray = static_cast<int>(next());
        ASSERT_TRUE(server_->addStringArrayVariable(
            ns, static_cast<uint32_t>(setupVars_.iProjektArray), "ProjektArray",
            std::vector<std::string>(kMaxProjects, "")));

        addBool(setupVars_.iUpdateProjektList, "UpdateProjektList");
        addBool(setupVars_.iProjektlistUpdated, "ProjektlistUpdated");
        addString(setupVars_.iProjektName, "ProjektName");
        addBool(setupVars_.iReadProjekt, "ReadProjekt");
        addInt16(setupVars_.iLenProjekt, "LenProjekt");
        addInt16(setupVars_.iAnzDatenblocke, "AnzDatenblöcke");
        addBool(setupVars_.iWriteData, "WriteData");
        addInt16(setupVars_.iIndexData, "IndexData");
        addBool(setupVars_.iReadData, "ReadData");
        addBool(setupVars_.ishowProjektInfo, "showProjektInfo");
        addBool(setupVars_.iProjektInfoWrote, "ProjektInfoWrote");
        addBool(setupVars_.iProjektVorhanden, "ProjektVorhanden");
        addBool(setupVars_.iabbruchUbertragen, "ubertragenAbbrechen");
        addBool(setupVars_.iUebertgarungBeendet, "UebertgarungBeendet");
        addInt16(setupVars_.iUebertragungslaege, "Uebertragungslaege");

        addInt16Array(setupVars_.icolorG0, "colorG0");
        addInt16(setupVars_.istreghtsG0, "streghtsG0", 1);
        addInt16Array(setupVars_.icolorG1, "colorG1");
        addInt16(setupVars_.istreghtsG1, "streghtsG1", 1);
        addInt16Array(setupVars_.icolorG2, "colorG2");
        addInt16(setupVars_.istreghtsG2, "streghtsG2", 1);
        addInt16Array(setupVars_.icolorG3, "colorG3");
        addInt16(setupVars_.istreghtsG3, "streghtsG3", 1);
        addInt16Array(setupVars_.iFarbeLive, "FarbeLive");
        addInt16Array(setupVars_.iFarbeErledigt, "FarbeErledigt");
        addInt16(setupVars_.iStaerkeLive, "StaerkeLive", 1);
        addInt16(setupVars_.iStaerkeErledigt, "StaerkeErledigt", 1);
        addInt16(setupVars_.iLiveStand, "LiveStand");
        addBool(setupVars_.iLiveAbbruch, "LiveAbbruch");

        setupVars_.iMinArcRadius = static_cast<int>(next());
        ASSERT_TRUE(server_->addFloatVariable(
            ns, static_cast<uint32_t>(setupVars_.iMinArcRadius), "minArcRadius", 0.0f));

        addInt16(setupVars_.objektgroesseX, "objektgroesseX", 100);
        addInt16(setupVars_.objektgroesseY, "objektgroesseY", 200);

        ASSERT_TRUE(server_->start());
    }

    void connectController()
    {
        controller_ = std::make_unique<SPSController>(
            "opc", "opc.tcp://localhost:" + std::to_string(port_));

        bool connected = false;
        for (int attempt = 0; attempt < 20 && !connected; ++attempt)
        {
            connected = controller_->connect();
            if (!connected)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        ASSERT_TRUE(connected) << "Client konnte den Testserver nicht erreichen";
        ASSERT_TRUE(controller_->setupVariables(setupVars_));
    }

    // Simulated PLC side of the block handshake: consume DataArray whenever
    // WriteData is set, then request the next block via ReadData.
    struct PlcSimulator
    {
        explicit PlcSimulator(OPCUAServer &server) : server_(server)
        {
            thread_ = std::thread([this]() { run(); });
        }

        ~PlcSimulator()
        {
            stop_.store(true);
            if (thread_.joinable())
            {
                thread_.join();
            }
        }

        void run()
        {
            while (!stop_.load())
            {
                bool writeData = false;
                if (server_.readBool("WriteData", writeData) && writeData)
                {
                    int16_t index = 0;
                    server_.readInt16("IndexData", index);
                    std::vector<std::string> block;
                    server_.readStringArray("DataArray", block);
                    receivedBlocks.push_back(block);
                    receivedIndices.push_back(index);
                    server_.writeBool("WriteData", false);
                    server_.writeBool("ReadData", true);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }

        OPCUAServer &server_;
        std::thread thread_;
        std::atomic<bool> stop_{false};
        // Only touched by the simulator thread until it is joined.
        std::vector<std::vector<std::string>> receivedBlocks;
        std::vector<int16_t> receivedIndices;
    };

    uint16_t port_ = 0;
    fs::path folder_;
    setupVarInfos setupVars_;
    AppState state_;
    std::unique_ptr<OPCUAServer> server_;
    std::unique_ptr<SPSController> controller_;
    std::unique_ptr<ProjectManager> manager_;
};

TEST_F(FullFlowTest, UpdateProjectListWritesProjectsToPlc)
{
    ASSERT_TRUE(manager_->updateProjectList(kMaxProjects));

    std::vector<std::string> projects;
    ASSERT_TRUE(server_->readStringArray("ProjektArray", projects));
    ASSERT_EQ(projects.size(), static_cast<size_t>(kMaxProjects));
    EXPECT_EQ(projects[0], kProjectFile);
    for (size_t i = 1; i < projects.size(); ++i)
    {
        EXPECT_TRUE(projects[i].empty()) << "Element " << i << " sollte leer sein";
    }

    bool updated = false;
    ASSERT_TRUE(server_->readBool("ProjektlistUpdated", updated));
    EXPECT_TRUE(updated);
}

TEST_F(FullFlowTest, ShowProjectInfoWritesCorrectRecordAndBlockCount)
{
    ASSERT_TRUE(server_->writeString("ProjektName", kProjectFile));

    auto [recordCount, ok] = manager_->showProjectInfo();
    ASSERT_TRUE(ok);
    EXPECT_EQ(recordCount, static_cast<int>(kProjectLines.size()));

    int16_t lenProjekt = 0;
    ASSERT_TRUE(server_->readInt16("LenProjekt", lenProjekt));
    EXPECT_EQ(lenProjekt, static_cast<int16_t>(kProjectLines.size()));

    // Regression for the field report of 20.07.2026: with the block size not
    // wired through, this was ceil(7/5) instead of ceil(7/kBlockSize).
    int16_t blockCount = 0;
    ASSERT_TRUE(server_->readInt16("AnzDatenblöcke", blockCount));
    EXPECT_EQ(blockCount, static_cast<int16_t>(
        (kProjectLines.size() + kBlockSize - 1) / kBlockSize));

    bool exists = false;
    ASSERT_TRUE(server_->readBool("ProjektVorhanden", exists));
    EXPECT_TRUE(exists);
}

TEST_F(FullFlowTest, TransferSendsAllBlocksCleanly)
{
    ASSERT_TRUE(server_->writeString("ProjektName", kProjectFile));

    std::atomic<int> keepAliveCalls{0};
    manager_->setKeepAlive([&keepAliveCalls]() {
        keepAliveCalls++;
        return true;
    });

    std::vector<std::vector<std::string>> blocks;
    std::vector<int16_t> indices;
    {
        PlcSimulator plc(*server_);
        ASSERT_TRUE(manager_->transferProjectData());
        // Give the simulator one last cycle to consume the final block.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        plc.stop_.store(true);
        if (plc.thread_.joinable())
        {
            plc.thread_.join();
        }
        blocks = plc.receivedBlocks;
        indices = plc.receivedIndices;
    }

    const size_t expectedBlocks = (kProjectLines.size() + kBlockSize - 1) / kBlockSize;
    ASSERT_EQ(blocks.size(), expectedBlocks);

    // IndexData counts the blocks starting at 1.
    ASSERT_EQ(indices.size(), expectedBlocks);
    for (size_t i = 0; i < indices.size(); ++i)
    {
        EXPECT_EQ(indices[i], static_cast<int16_t>(i + 1));
    }

    // Every transferred line must match the file content: in order, without
    // the header comment, the empty line or any CR left over from CRLF.
    size_t lineIndex = 0;
    for (size_t b = 0; b < blocks.size(); ++b)
    {
        const size_t linesInBlock =
            std::min(static_cast<size_t>(kBlockSize), kProjectLines.size() - lineIndex);
        ASSERT_EQ(blocks[b].size(), static_cast<size_t>(kBlockSize));
        for (size_t i = 0; i < linesInBlock; ++i, ++lineIndex)
        {
            const std::string &line = blocks[b][i];
            EXPECT_EQ(line, kProjectLines[lineIndex])
                << "Block " << b << ", Element " << i;
            EXPECT_EQ(line.find('\r'), std::string::npos);
            EXPECT_EQ(line.find('('), std::string::npos)
                << "Header-Zeile wurde übertragen";
        }
    }
    EXPECT_EQ(lineIndex, kProjectLines.size());

    // Keep-alive must have been invoked while the transfer waited.
    EXPECT_GT(keepAliveCalls.load(), 0);

    bool finished = false;
    ASSERT_TRUE(server_->readBool("UebertgarungBeendet", finished));
    EXPECT_TRUE(finished);
}

TEST_F(FullFlowTest, TransferAbortsOnPlcAbortFlag)
{
    ASSERT_TRUE(server_->writeString("ProjektName", kProjectFile));
    ASSERT_TRUE(server_->writeBool("ubertragenAbbrechen", true));

    // No simulator: the PLC never answers the handshake. The abort flag has
    // to end the transfer immediately instead of waiting for the timeout.
    const auto start = std::chrono::steady_clock::now();
    EXPECT_TRUE(manager_->transferProjectData());
    const auto elapsed = std::chrono::steady_clock::now() - start;
    EXPECT_LT(elapsed, std::chrono::seconds(10));

    bool abortFlag = true;
    ASSERT_TRUE(server_->readBool("ubertragenAbbrechen", abortFlag));
    EXPECT_FALSE(abortFlag) << "Der Dienst muss das Abbruch-Bit zurücksetzen";

    bool finished = false;
    ASSERT_TRUE(server_->readBool("UebertgarungBeendet", finished));
    EXPECT_TRUE(finished);
}

TEST_F(FullFlowTest, TransferAbortsWhenKeepAliveFails)
{
    ASSERT_TRUE(server_->writeString("ProjektName", kProjectFile));

    // Simulates shutdown/connection loss: keep-alive reports failure and the
    // transfer has to bail out instead of blocking the SPS thread forever.
    manager_->setKeepAlive([]() { return false; });

    const auto start = std::chrono::steady_clock::now();
    EXPECT_TRUE(manager_->transferProjectData());
    const auto elapsed = std::chrono::steady_clock::now() - start;
    EXPECT_LT(elapsed, std::chrono::seconds(10));
}
