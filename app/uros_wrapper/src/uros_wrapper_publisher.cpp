#include <string.h>
#include <uros_wrapper_publisher.hpp>


// Main constructor
UrosWrapperPublisher::UrosWrapperPublisher() {

}

void UrosWrapperPublisher::vInitPublisher(
    rcl_publisher_t xPublisher,
    const char* pcTopicName
) {
    this->xPublisher = xPublisher;
    strcpy(this->pcTopicName, pcTopicName);
}

// Setters
void UrosWrapperPublisher::vSetTopicName(const char* pcTopicName) {
    strcpy(this->pcTopicName, pcTopicName);
}

// Getters
const char* UrosWrapperPublisher::pcGetTopicName() {
    return this->pcTopicName;
}

rcl_publisher_t* UrosWrapperPublisher::xGetPublisher() {
    return &(this->xPublisher);
}
