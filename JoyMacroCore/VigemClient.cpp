#include "VigemClient.h"
#include "VigemController.h"

bool VigemClient::Initialize()
{
    LOG_DEBUG("Initializing Vigem Bus-Driver");
    client = vigem_alloc();
    if (client == nullptr)
    {
        LOG_ERROR("ERROR: Not enough memory to allocate VIGEM driver connection");
        return false;
    }

    // Make connection with bus driver
    const auto retval = vigem_connect(client);
    if (!VIGEM_SUCCESS(retval))
    {
        std::ostringstream oss; oss << "ERROR: ViGEm Bus connection failed with error code: 0x" << std::hex << retval << std::endl;
        LOG_ERROR(oss.str());
        vigem_free(client); // free client memory
        return false;
    }
    initted = true;
    LOG_DEBUG("Initialized Vigem Bus-Driver Successfully");
    return true;
}

bool VigemClient::CreateAndPlugController(int physicalControllerIndex)
{
    LOG_DEBUG("Attempting to create and plug virtual controller");
    if (_controller != nullptr) {
        return false; // controller already plugged in
    }

    // connect pad
    const auto pad = vigem_target_x360_alloc();
    // plugin pad
    const auto pir = vigem_target_add(client, pad);

    //
    // Error handling
    //
    if (!VIGEM_SUCCESS(pir))
    {
        std::ostringstream oss; oss << "ERROR: Target plugin failed with error code: 0x" << std::hex << pir << std::endl;
        LOG_ERROR(oss.str());
        vigem_target_free(pad);
        return false;
    }

    _controller = std::make_unique<VigemController>(this, pad, physicalControllerIndex);
    LOG_DEBUG("Successfully created and plugged in virtual controller");
    return true;
}

bool VigemClient::DeleteAndUnplugController()
{
    if (_controller != nullptr)
        return false;

    _controller = nullptr; // reset pointer, will trigger destructor
    return true;
}

VigemController* VigemClient::getControllerRef()
{
    return _controller.get();
}

VigemClient::~VigemClient()
{
    LOG_DEBUG("De-initializing VigemClient");
    if (initted) {
        vigem_disconnect(client);
        vigem_free(client);
    }
}
